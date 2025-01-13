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
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"
#include "rapidassist/macros.h"

#include <string>

#ifdef _WIN32
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN 1
//#   endif
#   include <Windows.h> // for GetModuleHandleEx()
#   include "rapidassist/undef_windows_macros.h"
#   include <psapi.h>
#   pragma comment( lib, "psapi.lib" )
#   include <Tlhelp32.h>
#elif defined(__linux__) || defined(__APPLE__)
#   include <unistd.h>
#   include <limits.h>
#   include <sys/types.h>
#   include <signal.h>
#   include <spawn.h>
#   include <sys/wait.h>
#   include <errno.h>
extern char **environ;
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>  // for _NSGetExecutablePath()
#include <libproc.h>      // for proc_listpids()
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
  bool GetThreadIds(const processid_t & pid, ProcessIdList & tids) {
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

  ExitCodeResult GetWin32ExitCodeResult(const processid_t & pid, DWORD & code) {
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

  bool FindProcessWindows(const processid_t & pid, HwndList & windows) {
    windows.clear();

    FindProcessWindowsStruct s;
    s.windows_ptr = &windows;
    s.pid = pid;

    bool success = (EnumWindows(EnumWindowsProc, (LPARAM)&s) == TRUE);
    return success;
  }

  bool CloseWindows(const processid_t & pid) {
    HwndList hWnds;
    bool success = FindProcessWindows(pid, hWnds);
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

  bool Terminate(const processid_t & pid, DWORD timeout_ms) {
    bool success = false;

    //Get a handle
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess) {
      ProcessIdList thread_ids;
      if (GetThreadIds(pid, thread_ids)) {
        DWORD num_threads = (DWORD)thread_ids.size();
        if (num_threads >= 1) {
          if (timeout_ms != INFINITE) {

            //Call WM_CLOSE & WM_QUIT on all the threads
            DWORD thread_timeout_ms = timeout_ms / num_threads;
            for (size_t thread_index = 0; thread_index < num_threads && !success; thread_index++) {
              DWORD thread_id = thread_ids[thread_index];
              bool post_success = (PostThreadMessage(thread_id, WM_CLOSE, 0, 0) != 0); //WM_CLOSE does not always work
              post_success = post_success && (PostThreadMessage(thread_id, WM_QUIT, 0, 0) != 0);
              if (post_success) {
                DWORD wait_result = WaitForSingleObject(hProcess, thread_timeout_ms);
                success = (wait_result == WAIT_OBJECT_0);

                //Some app does not signal the thread that accepted the WM_CLOSE or WM_QUIT messages
                if (!success)
                  success = !IsRunning(pid);

                //Some app needs to have their windows closed
                CloseWindows(pid);
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
                    success = !IsRunning(pid);

                  //Some app needs to have their windows closed
                  CloseWindows(pid);
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

#elif defined(__linux__)
  ///=========================================================================================
  ///                                 Linux support functions
  ///=========================================================================================

  /// <summary>
  /// Get the process state of the given process id.
  /// </summary>
  /// <param name="pid">The process id of the process.</param>
  /// <param name="state">The process state of the given process id.</param>
  /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
  bool GetProcessState(const processid_t & pid, char & state) {
    std::string stat_path = std::string("/proc/") + ra::strings::ToString(pid) + "/stat";
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
    ra::strings::Split(tokens, buffer, ' ');

    //assert the Minimum number of tokens
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
  bool IsRunningState(const char state) {
    // See GetProcessState() for known process states.
    bool running = (state == 'D' || state == 'R' || state == 'S');
    return running;
  }

  /// <summary>
  /// Verify if the given process id is a zombie process.
  /// </summary>
  /// <param name="pid">The process id of the process.</param>
  /// <returns>Returns true if the process id is a zombie process. Returns false otherwise.</returns>
  bool IsZombieProcess(const processid_t & pid) {
    //read the state of the given process
    char state = '\0';
    if (!GetProcessState(pid, state))
      return false; //failure to get process state

    // See GetProcessState() for known process states.
    bool zombie = (state == 'Z');
    return zombie;
  }

#endif

  std::string ToString(const ProcessIdList & processes) {
    std::string s;
    for (size_t i = 0; i < processes.size(); i++) {
      processid_t pid = processes[i];
      if (!s.empty())
        s.append(", ");
      s += ra::strings::ToString(pid);
    }
    return s;
  }

  std::string GetCurrentProcessPath() {
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
#elif defined(__linux__)
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
      snprintf(process_id_path, MAX_CHARACTERS_COUNT(process_id_path), "/proc/%d/exe", getpid());
      len = ::readlink(process_id_path, exe_path, sizeof(exe_path));
      if (len == -1 || len == sizeof(exe_path))
        len = 0;
      exe_path[len] = '\0';
      path = exe_path;
    }
#elif defined(__APPLE__)
    #if 0
    // Note: With the following implementation, calling function `GetCurrentProcessPath()`
    // returns the value `/Users/antoine/dev/RapidAssist/build/bin/./rapidassist_unittest-d`
    // which includes the string `/./` which is annoying.
    // Another implementation is preferred.

    //https://stackoverflow.com/questions/7004401/c-find-execution-path-on-mac

    // Get required buffer size
    uint32_t bufsize = 0;
    if (_NSGetExecutablePath(NULL, &bufsize) != -1)
      return ""; //fail to get the required size
    if (bufsize == 0)
      return ""; // fail to get required buffer size

    // Allocate memory
    char * buffer = NULL;
    buffer = new char[bufsize];
    if (!buffer)
      return ""; // Fail, not enough memory
    
    // Get actual executable path
    if(_NSGetExecutablePath(buffer, &bufsize) == 0) {
      path = buffer;
    }

    // Free memory
    delete[] buffer;
    #else
    // Note: With the following implementation, calling function `GetCurrentProcessPath()`
    // returns the value `/Users/antoine/dev/RapidAssist/build/bin/rapidassist_unittest-d`
    // which is the expected value.
    
    struct proc_bsdinfo proc;
    char tmp[4096]; // Using a bigger buffer results in the function `proc_pidpath()` failing to execute and returning an empty string.
    tmp[0] = '\0';
    pid_t pid = getpid();
    int path_size = proc_pidpath(pid, tmp, sizeof(tmp));
    if (path_size > 0 && tmp[0] != '\0')
      path = tmp;
    #endif
#endif
    return path;
  }

  ProcessIdList GetProcesses() {
    ProcessIdList processes;
#ifdef _WIN32
    //Get process ids
    const int MAX_PROCESSES = 10240;
    DWORD process_ids[MAX_PROCESSES];
    DWORD process_ids_size = 0; //in bytes
    EnumProcesses(process_ids, MAX_PROCESSES, &process_ids_size);
    DWORD num_processes = process_ids_size / sizeof(DWORD);

    //for each process
    for (unsigned int i = 0; i < num_processes; i++) {
      DWORD pid = process_ids[i];
      processes.push_back(pid);
    }
#elif defined(__linux__)
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
      bool numeric = ra::strings::IsNumeric(name.c_str());
      if (!numeric)
        continue;

      //that's a process id. Parse it
      processid_t pid = INVALID_PROCESS_ID;
      bool parsed_ok = ra::strings::Parse(name.c_str(), pid);
      if (!parsed_ok)
        continue;

      //filter out process id that are not running
      //(i.e. zombie processes)
      char state = '\0';
      if (!GetProcessState(pid, state))
        continue;
      bool running = IsRunningState(state);
      if (!running)
        continue;

      processes.push_back(pid);
    }
#elif defined(__APPLE__)
    //https://stackoverflow.com/questions/6045878/observe-a-process-of-unknown-pid-no-ui/6046282#6046282
    //https://stackoverflow.com/questions/49506579/how-to-find-the-pid-of-any-process-in-mac-osx-c
    const size_t MAX_PROCESSES = 10240;
    pid_t pids[MAX_PROCESSES];
    int bytes = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    int num_proc = bytes / sizeof(pids[0]);
    for (int i = 0; i < num_proc; i++) {
      struct proc_bsdinfo proc;
      int st = proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0, &proc, PROC_PIDTBSDINFO_SIZE);
      if (st == PROC_PIDTBSDINFO_SIZE) {
        processes.push_back(pids[i]);
      }       
    }
#endif
    return processes;
  }

  processid_t GetCurrentProcessId() {
#ifdef _WIN32
    processid_t pid = ::GetCurrentProcessId();
#else
    processid_t pid = getpid();
#endif
    return pid;
  }

  std::string GetCurrentProcessDir() {
    std::string dir;
    std::string exec_path = GetCurrentProcessPath();
    if (exec_path.empty())
      return dir; //failure
    dir = ra::filesystem::GetParentPath(exec_path);
    return dir;
  }

  processid_t StartProcess(const std::string & exec_path) {
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();

    // Launch the process from the current process current directory
    processid_t pid = StartProcess(exec_path, curr_dir);
    return pid;
  }

  processid_t StartProcess(const std::string & exec_path, const std::string & default_directory) {
    // Launch the process with no arguments
#ifdef _WIN32
    processid_t pid = StartProcess(exec_path, default_directory, "");
    return pid;
#else
    ra::strings::StringVector args;
    processid_t pid = StartProcess(exec_path, default_directory, args);
    return pid;
#endif
  }

#ifdef _WIN32
  processid_t StartProcess(const std::string & exec_path, const std::string & default_directory, const std::string & command_line) {
    //build the full command line
    std::string command;

    //handle exec_path
    if (!exec_path.empty()) {
      if (exec_path.find(" ") != std::string::npos) {
        command += "\"";
        command += exec_path;
        command += "\"";
      }
      else
        command += exec_path;
    }

    if (!command.empty()) {
      command += " ";
      command += command_line;
    }

    //launch a new process with the command line
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFO startup_info = { 0 };
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOWDEFAULT; //SW_SHOW, SW_SHOWNORMAL
    static const DWORD creation_flags = 0; //EXTENDED_STARTUPINFO_PRESENT
    bool success = (CreateProcess(NULL, (char*)command.c_str(), NULL, NULL, FALSE, creation_flags, NULL, default_directory.c_str(), &startup_info, &process_info) != 0);
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
  processid_t StartProcess(const std::string & exec_path, const std::string & default_directory, const ra::strings::StringVector & arguments) {
    //temporary change the current directory for the child process
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();
    if (!ra::filesystem::DirectoryExists(default_directory.c_str()))
      return INVALID_PROCESS_ID;
    int chdir_result = chdir(default_directory.c_str());

    //prepare argv
    //the first element of argv must be the executable path itself.
    //the last element of argv must be am empty argument
    static const int MAX_ARGUMENTS = 10240;
    char * argv[MAX_ARGUMENTS] = { 0 };
    argv[0] = (char*)exec_path.c_str();
    for (size_t i = 0; i < arguments.size() && i < (MAX_ARGUMENTS - 2); i++) {
      char * arg_value = (char*)arguments[i].c_str();
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
    int status = posix_spawn(&child_pid, exec_path.c_str(), NULL, NULL, argv, environ);
    if (status != 0)
      child_pid = INVALID_PROCESS_ID;

    //restore current directory back to the previous location
    chdir_result = chdir(curr_dir.c_str());

    return child_pid;
  }
#endif

  bool OpenDocument(const std::string & path) {
    if (!ra::filesystem::FileExists(path.c_str()))
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
    info.lpFile = path.c_str();
    info.lpParameters = NULL; //arguments
    info.lpDirectory = NULL; // default directory

    BOOL success = ShellExecuteEx(&info);
    if (success) {
      HANDLE hProcess = info.hProcess;
      DWORD pid = GetProcessId(hProcess);
      return true;
    }
    return false;
#elif defined(__linux__) || defined(__APPLE__)
    #if defined(__linux__)
    const char * open_path = "/usr/bin/xdg-open";
    #elif defined(__APPLE__)
    const char * open_path = "/usr/bin/open";
    #endif
    if (!ra::filesystem::FileExists(open_path))
      return false; //open or xdg-open not found

    ra::strings::StringVector args;
    args.push_back(path);
    std::string curr_dir = ra::filesystem::GetCurrentDirectory();
    processid_t pid = StartProcess(open_path, curr_dir, args);
    bool success = (pid != INVALID_PROCESS_ID);
    return success;
#endif
  }

  bool Kill(const processid_t & pid) {
    bool success = false;
#ifdef _WIN32
    //Get a handle
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess) {
      success = (TerminateProcess(hProcess, 255) != 0);
      CloseHandle(hProcess);
    }
#elif defined(__linux__) || defined(__APPLE__)
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

  bool IsRunning(const processid_t & pid) {
#ifdef _WIN32
    DWORD exit_code = 0;
    ExitCodeResult result = GetWin32ExitCodeResult(pid, exit_code);
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
      ProcessIdList processes = GetProcesses();
      for (size_t i = 0; i < processes.size() && running == false; i++) {
        DWORD tmp_pid = processes[i];
        if (tmp_pid == pid)
          running = true;
      }
    }
    break;
    default:
      running = false; //should not append unless GetWin32ExitCodeResult is modified without notice.
    };
    return running;
#elif defined(__linux__)
    char state = '\0';
    if (!GetProcessState(pid, state))
      return false; //unable to find process state

    // See GetProcessState() for known process states.
    bool running = IsRunningState(state);
    return running;
#elif defined(__APPLE__)
    //https://stackoverflow.com/questions/49506579/how-to-find-the-pid-of-any-process-in-mac-osx-c
    struct proc_bsdinfo proc;
    int st = proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &proc, PROC_PIDTBSDINFO_SIZE);
    if (st == PROC_PIDTBSDINFO_SIZE) {
      return true;
    }
    // Failed to get bsd information about process.
    // Most probable reason is that process is not created by current user.

    //Try to get the process path
    char path[1024];
    int path_size = proc_pidpath(pid, path, sizeof(path));
    if (path_size > 0 && path[0] != '\0')
      return true;

    return false;
#endif
  }

  bool Terminate(const processid_t & pid) {
#ifdef _WIN32
    //ask the process to exit gracefully allowing a maximum of 60 seconds to close
    bool terminated = Terminate(pid, 60000);
    return terminated;
#elif defined(__linux__) || defined(__APPLE__)
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

  bool GetExitCode(const processid_t & pid, int & exit_code) {
#ifdef _WIN32
    DWORD local_exit_code;
    ExitCodeResult result = GetWin32ExitCodeResult(pid, local_exit_code);
    if (result == EXIT_CODE_SUCCESS) {
      exit_code = static_cast<int>(local_exit_code);
      return true;
    }
    return false;
#elif defined(__linux__) || defined(__APPLE__)
    int status = 0;
    pid_t results_pid = waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (results_pid == pid) {
      //waitpid success
      bool process_exited = WIFEXITED(status);
      exit_code = WEXITSTATUS(status);
      return true;
    }
    return false;
#endif
  }

  bool WaitExit(const processid_t & pid) {
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
#elif defined(__linux__) || defined(__APPLE__)
    //DISABLED THE FOLLOWING IMPLEMENTATION:
    //  waitpid() function consumes the process exit code which disables the implementation of GetExitCode().
    //  In other words, calling GetExitCode() will always fails after calling the waitpid() function.
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
    //    ra::timing::Millisleep(100);
    //    res = ::kill(pid, 0);
    //  }

    //validate if pid is valid
    int res = ::kill(pid, 0);
    bool valid_pid = (res == 0 || (res < 0 && errno == EPERM));
    if (!valid_pid) {
      return false;
    }

    //wait for the process state to change
    //this implementation is slow but does not rely on waitpid()
    //to detect the end of the process
    while (IsRunning(pid)) {
      //wait a little more and verify again
      ra::timing::Millisleep(1000);
    }

    return true;
#endif
  }

  bool WaitExit(const processid_t & pid, int & exit_code) {
    bool success = WaitExit(pid);
    if (!success) {
      return false;
    }

#ifndef _WIN32
    //also read the process exit code to remove the zombie process
    success = GetExitCode(pid, exit_code);
    if (!success) {
      return false;
    }
#endif

    return success;
  }

} //namespace process
} //namespace ra
