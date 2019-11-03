/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "rapidassist/process.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/timing.h"

#include <string>

#ifdef WIN32
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN 1
//#   endif
#   include <windows.h> // for GetModuleHandleEx()
#   include <psapi.h>
#   pragma comment( lib, "psapi.lib" )
#   include <Tlhelp32.h>
#elif __linux__
#   include <unistd.h>
#   include <limits.h>
#   include <sys/types.h>
#   include <signal.h>
#   include <spawn.h>
#   include <sys/wait.h>
#   include <errno.h>
extern char **environ;
#endif

namespace ra { namespace process {

  /// <summary>
  /// Define invalid process id.
  /// Note:
  ///   On win32 platform, an invalid process id is defined as 0. See the following reference for details:
  ///     - https://stackoverflow.com/questions/3232401/windows-pid-0-valid
  ///     - https://stackoverflow.com/questions/26993596/getprocessid-returning-zero/26993697
  ///     - https://devblogs.microsoft.com/oldnewthing/20040223-00/?p=40503
  ///   On linux plarform, an invalid process id is defined as 0. See the following reference for details:
  ///     - https://serverfault.com/questions/279178/what-is-the-range-of-a-pid-on-linux-and-solaris
  ///     - https://serverfault.com/a/279180
  /// </summary>
  const processid_t INVALID_PROCESS_ID = (processid_t)-1;


#ifdef _WIN32
  ///=========================================================================================
  ///                                 WIN32 support functions
  ///=========================================================================================

  /// <summary>
  /// Get the list of threads of a process.
  /// </summary>
  /// <param name="pid">The process id of the process.</param>
  /// <param name="tids">The list of thread ids of the process.</param>
  /// <returns>Returns true if the list of thread is returned. Returns false otherwise.</returns>
  bool getThreadIds(const processid_t & pid, ProcessIdList & tids) {
    tids.clear();

    //Getting threads id of the process
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 thread_entry;

    // Take a snapshot of all running threads
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
      return false;

    // Fill in the size of the structure before using it.
    thread_entry.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(hThreadSnap, &thread_entry)) {
      CloseHandle(hThreadSnap); // clean the snapshot object
      return false;
    }

    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    do {
      if (thread_entry.th32OwnerProcessID == pid) {
        //printf( "\n\n     THREAD ID      = 0x%08X", thread_entry.th32ThreadID );
        //printf( "\n     Base priority  = %d", thread_entry.tpBasePri );
        //printf( "\n     Delta priority = %d", thread_entry.tpDeltaPri );
        tids.push_back(thread_entry.th32ThreadID);
      }
    } while (Thread32Next(hThreadSnap, &thread_entry));

    return true;
  }

  enum ExitCodeResult {
    EXIT_CODE_SUCCESS,
    EXIT_CODE_STILLRUNNING,
    EXIT_CODE_FAILED
  };

  ExitCodeResult getWin32ExitCodeResult(const processid_t & pid, DWORD & code) {
    ExitCodeResult result = EXIT_CODE_FAILED;

    //Get a handle
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
      DWORD exit_code = 0;
      if (::GetExitCodeProcess(hProcess, &exit_code)) {
        if (exit_code != STILL_ACTIVE) {
          result = EXIT_CODE_SUCCESS;
        }
        else {
          //Check if process is still alive
          DWORD wait_result = ::WaitForSingleObject(hProcess, 0);
          if (wait_result == WAIT_OBJECT_0) {
            result = EXIT_CODE_SUCCESS;
          }
          else if (wait_result == WAIT_TIMEOUT) {
            result = EXIT_CODE_STILLRUNNING;
          }
          else {
            //Error
            result = EXIT_CODE_FAILED;
          }
        }
      }

      CloseHandle(hProcess);

      bool success = (result == EXIT_CODE_SUCCESS);
      if (success)
        code = exit_code;
    }
    return result;
  }

  typedef std::vector<HWND> HwndList;

  struct FindProcessWindowsStruct {
    HwndList * windows_ptr;
    processid_t pid;
  };

  BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD process_id = 0;

    if (!hWnd)
      return TRUE;		// Not a window
    if (lParam == NULL)
      return TRUE;    // No FindProcessWindowsStruct pointer provided

    FindProcessWindowsStruct & s = (*((FindProcessWindowsStruct*)lParam));

    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    if (hInstance) {
      DWORD thread_id = GetWindowThreadProcessId(hWnd, &process_id);
      if (thread_id) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
        if (hProcess) {
          //is this the process we are looking for ?
          if (process_id == s.pid) {
            //add found window handle to list
            s.windows_ptr->push_back(hWnd);
          }
        }
        CloseHandle(hProcess);
      }
    }
    return TRUE;
  }

  bool findProcessWindows(const processid_t & pid, HwndList & oWindows) {
    oWindows.clear();

    FindProcessWindowsStruct s;
    s.windows_ptr = &oWindows;
    s.pid = pid;

    bool success = (EnumWindows(EnumWindowsProc, (LPARAM)&s) == TRUE);
    return success;
  }

  bool closeWindows(const processid_t & pid) {
    HwndList hWnds;
    bool success = findProcessWindows(pid, hWnds);
    if (success) {
      for (size_t i = 0; i < hWnds.size(); i++) {
        HWND hWnd = hWnds[i];
        //#define KEYPRESS_MACRO_FUNCTION PostMessage
        #define KEYPRESS_MACRO_FUNCTION SendMessage
        //success = success & (KEYPRESS_MACRO_FUNCTION(hWnd, WM_SYSKEYDOWN, VK_MENU, 0) == TRUE);
        //success = success & (KEYPRESS_MACRO_FUNCTION(hWnd, WM_SYSKEYDOWN, VK_F4, 0) == TRUE);
        //success = success && (KEYPRESS_MACRO_FUNCTION(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0) == TRUE);
        KEYPRESS_MACRO_FUNCTION(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
      }
      success = true;
    }
    return success;
  }

  bool terminate(const processid_t & pid, DWORD iTimeoutMS) {
    bool success = false;

    //Get a handle
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess) {
      ProcessIdList thread_ids;
      if (getThreadIds(pid, thread_ids)) {
        size_t num_threads = thread_ids.size();
        if (num_threads >= 1) {
          if (iTimeoutMS != INFINITE) {

            //Call WM_CLOSE & WM_QUIT on all the threads
            size_t thread_timeout_ms = iTimeoutMS / num_threads;
            for (size_t thread_index = 0; thread_index < num_threads && !success; thread_index++) {
              DWORD thread_id = thread_ids[thread_index];
              bool post_success = (PostThreadMessage(thread_id, WM_CLOSE, 0, 0) != 0); //WM_CLOSE does not always work
              post_success = post_success && (PostThreadMessage(thread_id, WM_QUIT, 0, 0) != 0);
              if (post_success) {
                DWORD wait_result = WaitForSingleObject(hProcess, thread_timeout_ms);
                success = (wait_result == WAIT_OBJECT_0);

                //Some app does not signal the thread that accepted the WM_CLOSE or WM_QUIT messages
                if (!success)
                  success = !isRunning(pid);

                //Some app needs to have their windows closed
                closeWindows(pid);
              }
            }
          }
          else {
            //Call WM_CLOSE & WM_QUIT on all the threads
            while (!success) {
              for (size_t thread_index = 0; thread_index < num_threads && !success; thread_index++) {
                DWORD thread_id = thread_ids[thread_index];
                bool post_success = (PostThreadMessage(thread_id, WM_CLOSE, 0, 0) != 0); //WM_CLOSE does not always work
                post_success = post_success && (PostThreadMessage(thread_id, WM_QUIT, 0, 0) != 0);
                if (post_success) {
                  DWORD wait_result = WaitForSingleObject(hProcess, 200);
                  success = (wait_result == WAIT_OBJECT_0);

                  //Some app does not signal the thread that accepted the WM_CLOSE or WM_QUIT messages
                  if (!success)
                    success = !isRunning(pid);

                  //Some app needs to have their windows closed
                  closeWindows(pid);
                }
              }
            }
          }
        }
      }
      CloseHandle(hProcess);
    }

    return success;
  }

#else
  ///=========================================================================================
  ///                                 Linux support functions
  ///=========================================================================================

  /// <summary>
  /// Get the process state of the given process id.
  /// </summary>
  /// <param name="pid">The process id of the process.</param>
  /// <param name="state">The process state of the given process id.</param>
  /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
  bool getProcessState(const processid_t & pid, char & state) {
    std::string stat_path = std::string("/proc/") + ra::strings::toString(pid) + "/stat";
    bool exists = ra::filesystem::FileExists(stat_path.c_str());
    if (!exists)
      return false;

    FILE * f = fopen(stat_path.c_str(), "r");
    if (!f)
      return false;

    //read first 1024 bytes
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    char * tmp = fgets(buffer, BUFFER_SIZE, f);
    buffer[BUFFER_SIZE - 1] = '\0';
    fclose(f);

    //split each token into a list
    ra::strings::StringVector tokens;
    ra::strings::split(tokens, buffer, ' ');

    //assert the minimum number of tokens
    if (tokens.size() < 3)
      return false; //not enough tokens

    const std::string & pid_s = tokens[0];
    const std::string & name_s = tokens[1];
    const std::string & state_s = tokens[2];

    if (state_s.size() != 1)
      return false; //not a state

    //read the process state expecting one of the following characters:
    // D Uninterruptible sleep (usually IO)
    // R Running or runnable (on run queue)
    // S Interruptible sleep (waiting for an event to complete)
    // T Stopped, either by a job control signal or because it is being traced.
    // W paging (not valid since the 2.6.xx kernel)
    // X dead (should never be seen)
    // Z Defunct ("zombie") process, terminated but not reaped by its parent.
    // I ?????
    state = state_s[0];

    return true;
  }

  /// <summary>
  /// Define if a process is running or not.
  /// A zombie process is not considered running.
  /// </summary>
  /// <param name="state">The process state process.</param>
  /// <returns>Returns true if the process is running. Returns false otherwise.</returns>
  bool isRunningState(const char state) {
    // See getProcessState() for known process states.
    bool running = (state == 'D' || state == 'R' || state == 'S');
    return running;
  }

  /// <summary>
  /// Verify if the given process id is a zombie process.
  /// </summary>
  /// <param name="pid">The process id of the process.</param>
  /// <returns>Returns true if the process id is a zombie process. Returns false otherwise.</returns>
  bool isZombieProcess(const processid_t & pid) {
    //read the state of the given process
    char state = '\0';
    if (!getProcessState(pid, state))
      return false; //failure to get process state

    // See getProcessState() for known process states.
    bool zombie = (state == 'Z');
    return zombie;
  }

#endif

  std::string toString(const ProcessIdList & processes) {
    std::string s;
    for (size_t i = 0; i < processes.size(); i++) {
      processid_t pid = processes[i];
      if (!s.empty())
        s.append(", ");
      s += ra::strings::toString(pid);
    }
    return s;
  }

  std::string getCurrentProcessPath() {
    std::string path;
#ifdef _WIN32
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
      int ret = GetLastError();
      return path; //failure
    }
    //get the path of this process
    char buffer[MAX_PATH] = { 0 };
    if (!GetModuleFileName(hModule, buffer, sizeof(buffer))) {
      int ret = GetLastError();
      return path; //failure
    }
    path = buffer;
#elif __linux__
    //from https://stackoverflow.com/a/33249023
    char exe_path[PATH_MAX + 1] = { 0 };
    ssize_t len = ::readlink("/proc/self/exe", exe_path, sizeof(exe_path));
    if (len == -1 || len == sizeof(exe_path))
      len = 0;
    exe_path[len] = '\0';
    path = exe_path;

    //fallback from https://stackoverflow.com/a/7052225
    if (path.empty()) {
      char process_id_path[32];
      sprintf(process_id_path, "/proc/%d/exe", getpid());
      len = ::readlink(process_id_path, exe_path, sizeof(exe_path));
      if (len == -1 || len == sizeof(exe_path))
        len = 0;
      exe_path[len] = '\0';
      path = exe_path;
    }
#endif
    return path;
  }

  ProcessIdList getProcesses() {
    ProcessIdList processes;

#ifdef _WIN32
    //Get process ids
    const int MAX_PROCESSES = 10000;
    DWORD process_ids[MAX_PROCESSES];
    DWORD process_ids_size = 0; //in bytes
    EnumProcesses(process_ids, MAX_PROCESSES, &process_ids_size);
    DWORD num_processes = process_ids_size / sizeof(DWORD);

    //for each process
    for (unsigned int i = 0; i < num_processes; i++) {
      DWORD pid = process_ids[i];
      processes.push_back(pid);
    }
#else
    //list processes from the filesystem
    ra::strings::StringVector files;
    bool found = ra::filesystem::FindFiles(files, "/proc", 0);
    if (!found)
      return processes; //failed
    for (size_t i = 0; i < files.size(); i++) {
      const std::string & file = files[i];

      //filter out files
      bool is_directory = ra::filesystem::DirectoryExists(file.c_str());
      if (!is_directory)
        continue;

      //filter out directories that are not numeric.
      const std::string name = ra::filesystem::GetFilename(file.c_str());
      bool numeric = ra::strings::isNumeric(name.c_str());
      if (!numeric)
        continue;

      //that's a process id. Parse it
      processid_t pid = INVALID_PROCESS_ID;
      bool parsed_ok = ra::strings::parse(name.c_str(), pid);
      if (!parsed_ok)
        continue;

      //filter out process id that are not running
      //(i.e. zombie processes)
      char state = '\0';
      if (!getProcessState(pid, state))
        continue;
      bool running = isRunningState(state);
      if (!running)
        continue;

      processes.push_back(pid);
    }
#endif

    return processes;
  }

  processid_t getCurrentProcessId() {
#ifdef _WIN32
    processid_t pid = GetCurrentProcessId();
#else
    processid_t pid = getpid();
#endif
    return pid;
  }

  std::string getCurrentProcessDir() {
    std::string dir;
    std::string exec_path = getCurrentProcessPath();
    if (exec_path.empty())
      return dir; //failure
    dir = ra::filesystem::GetParentPath(exec_path);
    return dir;
  }

  processid_t startProcess(const std::string & iExecPath) {
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();

    // Launch the process from the current process current directory
    processid_t pid = startProcess(iExecPath, curr_dir);
    return pid;
  }

  processid_t startProcess(const std::string & iExecPath, const std::string & iDefaultDirectory) {
    // Launch the process with no arguments
#ifdef _WIN32
    processid_t pid = startProcess(iExecPath, iDefaultDirectory, "");
    return pid;
#else
    ra::strings::StringVector args;
    processid_t pid = startProcess(iExecPath, iDefaultDirectory, args);
    return pid;
#endif
  }

#ifdef _WIN32
  processid_t startProcess(const std::string & iExecPath, const std::string & iDefaultDirectory, const std::string & iCommandLine) {
    //build the full command line
    std::string command;

    //handle iExecPath
    if (!iExecPath.empty()) {
      if (iExecPath.find(" ") != std::string::npos) {
        command += "\"";
        command += iExecPath;
        command += "\"";
      }
      else
        command += iExecPath;
    }

    if (!command.empty()) {
      command += " ";
      command += iCommandLine;
    }

    //launch a new process with the command line
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFO startup_info = { 0 };
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOWDEFAULT; //SW_SHOW, SW_SHOWNORMAL
    static const DWORD creation_flags = 0; //EXTENDED_STARTUPINFO_PRESENT
    bool success = (CreateProcess(NULL, (char*)command.c_str(), NULL, NULL, FALSE, creation_flags, NULL, iDefaultDirectory.c_str(), &startup_info, &process_info) != 0);
    if (success) {
      //Wait for the application to initialize properly
      WaitForInputIdle(process_info.hProcess, INFINITE);

      //Extract the program id
      DWORD process_id = process_info.dwProcessId;

      //return the process id
      processid_t pId = static_cast<processid_t>(process_id);
      return pId;
    }
    return INVALID_PROCESS_ID;
  }
#else
  processid_t startProcess(const std::string & iExecPath, const std::string & iDefaultDirectory, const ra::strings::StringVector & iArguments) {
    //temporary change the current directory for the child process
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();
    if (!ra::filesystem::DirectoryExists(iDefaultDirectory.c_str()))
      return INVALID_PROCESS_ID;
    int chdir_result = chdir(iDefaultDirectory.c_str());

    //prepare argv
    //the first element of argv must be the executable path itself.
    //the last element of argv must be am empty argument
    static const int MAX_ARGUMENTS = 10240;
    char * argv[MAX_ARGUMENTS] = { 0 };
    argv[0] = (char*)iExecPath.c_str();
    for (size_t i = 0; i < iArguments.size() && i < (MAX_ARGUMENTS - 2); i++) {
      char * arg_value = (char*)iArguments[i].c_str();
      argv[i + 1] = arg_value;
    }

    //Print arguments.
    //printf("posix_spawn():\n");
    //fflush(NULL);
    //int i=0;
    //while(argv[i])
    //{
    //  printf("arg[%d]=%s\n", i, argv[i]);
    //  i++;
    //}

    pid_t child_pid = INVALID_PROCESS_ID;
    int status = posix_spawn(&child_pid, iExecPath.c_str(), NULL, NULL, argv, environ);
    if (status != 0)
      child_pid = INVALID_PROCESS_ID;

    //restore current directory back to the previous location
    chdir_result = chdir(curr_dir.c_str());

    return child_pid;
  }
#endif

  bool openDocument(const std::string & iPath) {
    if (!ra::filesystem::FileExists(iPath.c_str()))
      return false; //file not found

#ifdef _WIN32
    SHELLEXECUTEINFO info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFO);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = "open";
    info.lpFile = iPath.c_str();
    info.lpParameters = NULL; //arguments
    info.lpDirectory = NULL; // default directory

    BOOL success = ShellExecuteEx(&info);
    if (success) {
      HANDLE hProcess = info.hProcess;
      DWORD pid = GetProcessId(hProcess);
      return true;
    }
    return false;
#else
    const char * xdgopen_path = "/usr/bin/xdg-open";
    if (!ra::filesystem::FileExists(xdgopen_path))
      return false; //xdg-open not found

    const ra::strings::StringVector args;
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();
    processid_t pid = startProcess(xdgopen_path, curr_dir, args);
    bool success = (pid != INVALID_PROCESS_ID);
    return success;
#endif
  }

  bool kill(const processid_t & pid) {
    bool success = false;
#ifdef _WIN32
    //Get a handle
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess) {
      success = (TerminateProcess(hProcess, 255) != 0);
      CloseHandle(hProcess);
    }
#else
    int kill_error = ::kill(pid, SIGKILL);
    success = (kill_error == 0);

    if (success) {
      // call waitpid() on Linux to prevent having zombie processes.
      int status = 0;
      processid_t result_pid = waitpid(pid, &status, 0);
    }

#endif
    return success;
  }

  bool isRunning(const processid_t & pid) {
#ifdef _WIN32
    DWORD exit_code = 0;
    ExitCodeResult result = getWin32ExitCodeResult(pid, exit_code);
    bool running = false;
    switch (result) {
    case EXIT_CODE_SUCCESS:
      running = false;
      break;
    case EXIT_CODE_STILLRUNNING:
      running = true;
      break;
    case EXIT_CODE_FAILED:
    {
      //set the process as not running by default
      running = false;

      //search within existing processes
      ProcessIdList processes = getProcesses();
      for (size_t i = 0; i < processes.size() && running == false; i++) {
        DWORD tmp_pid = processes[i];
        if (tmp_pid == pid)
          running = true;
      }
    }
    break;
    default:
      running = false; //should not append unless getWin32ExitCodeResult is modified without notice.
    };
    return running;
#else
    char state = '\0';
    if (!getProcessState(pid, state))
      return false; //unable to find process state

    // See getProcessState() for known process states.
    bool running = isRunningState(state);
    return running;
#endif
  }

  bool terminate(const processid_t & pid) {
#ifdef _WIN32
    //ask the process to exit gracefully allowing a maximum of 60 seconds to close
    bool terminated = terminate(pid, 60000);
    return terminated;
#else
    //ask the process to exit gracefully
    int kill_error = ::kill(pid, SIGTERM);
    bool success = (kill_error == 0);

    if (success) {
      // call waitpid() on Linux to prevent having zombie processes.
      int status = 0;
      processid_t result_pid = waitpid(pid, &status, 0);
    }

    return success;
#endif
  }

  bool getExitCode(const processid_t & pid, int & exit_code) {
#ifdef _WIN32
    DWORD local_exit_code;
    ExitCodeResult result = getWin32ExitCodeResult(pid, local_exit_code);
    if (result == EXIT_CODE_SUCCESS) {
      exit_code = static_cast<int>(local_exit_code);
      return true;
    }
    return false;
#else
    int status = 0;
    if (waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED) == pid) {
      //waitpid success
      bool process_exited = WIFEXITED(status);
      exit_code = WEXITSTATUS(status);
      return true;
    }
    return false;
#endif
  }

  bool waitExit(const processid_t & pid) {
#ifdef _WIN32
    //Get a handle on the process
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, TRUE, pid);
    if (hProcess) {
      //now wait for the process termination
      WaitForSingleObject(hProcess, INFINITE);

      CloseHandle(hProcess);
      return true;
    }
    return false;
#else
    //DISABLED THE FOLLOWING IMPLEMENTATION:
    //  waitpid() function consumes the process exit code which disables the implementation of getExitCode().
    //  In other words, calling getExitCode() will always fails after calling the waitpid() function.
    //  This is why this function would have to also return the exit code.
    //  
    //  int status = 0;
    //  if (waitpid(pid, &status, 0) == pid)
    //  {
    //    //waitpid success
    //    bool process_exited = WIFEXITED( status );
    //    int exit_code = WEXITSTATUS( status );
    //    return true;
    //  }
    //  return false;

    //DISABLED THE FOLLOWING IMPLEMENTATION:
    //  Using kill() function to detect if a process is alive works great but it does not detect
    //  when a process is done executing and enters zombie state waiting for the user to call waitpid()
    //  to get the process exit code.
    //
    //  int res = ::kill(pid, 0);
    //  while (res == 0 || (res < 0 && errno == EPERM))
    //  {
    //    ra::timing::millisleep(100);
    //    res = ::kill(pid, 0);
    //  }

    //validate if pid is valid
    int res = ::kill(pid, 0);
    bool valid_pid = (res == 0 || (res < 0 && errno == EPERM));
    if (!valid_pid)
      return false;

    //wait for the process state to change
    //this implementation is slow but does not rely on waitpid()
    //to detect the end of the process
    while (isRunning(pid)) {
      //wait a little more and verify again
      ra::timing::millisleep(1000);
    }

    return true;
#endif
  }

  bool waitExit(const processid_t & pid, int & exit_code) {
    bool success = waitExit(pid);
    if (!success)
      return false;

#ifndef _WIN32
    //also read the process exit code to remove the zombie process
    success = getExitCode(pid, exit_code);
#endif

    return success;
  }

} //namespace process
} //namespace ra
