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
#endif

namespace ra
{
  namespace process
  {

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
    static const processid_t INVALID_PROCESS_ID = (processid_t)-1;

    std::string toString(const ProcessIdList & processes)
    {
      std::string s;
      for(size_t i=0; i<processes.size(); i++)
      {
        processid_t pid = processes[i];
        if (!s.empty())
          s.append(", ");
        s += ra::strings::toString(pid);
      }
      return s;
    }

    std::string getCurrentProcessPath()
    {
      std::string path;
#ifdef _WIN32
      HMODULE hModule = GetModuleHandle(NULL);
      if (hModule == NULL)
      {
        int ret = GetLastError();
        return path; //failure
      }
      //get the path of this process
      char buffer[MAX_PATH] = {0};
      if (!GetModuleFileName(hModule, buffer, sizeof(buffer)))
      {
        int ret = GetLastError();
        return path; //failure
      }
      path = buffer;
#elif __linux__
      //from https://stackoverflow.com/a/33249023
      char exePath[PATH_MAX + 1] = {0};
      ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
      if (len == -1 || len == sizeof(exePath))
        len = 0;
      exePath[len] = '\0';
      path = exePath;

      //fallback from https://stackoverflow.com/a/7052225
      if (path.empty())
      {
        char process_id_path[32];
        sprintf( process_id_path, "/proc/%d/exe", getpid() );
        len = ::readlink(process_id_path, exePath, sizeof(exePath));
        if (len == -1 || len == sizeof(exePath))
          len = 0;
        exePath[len] = '\0';
        path = exePath;
      }
#endif
      return path;
    }

    ProcessIdList getProcesses()
    {
      ProcessIdList processes;

#ifdef _WIN32
      //Get process ids
      const int MAX_PROCESSES = 10000;
      DWORD wProcessIds[MAX_PROCESSES];
      DWORD wProcessIdsSize = 0; //in bytes
      EnumProcesses(wProcessIds, MAX_PROCESSES, &wProcessIdsSize);
      DWORD wNumProcesses = wProcessIdsSize / sizeof(DWORD);

      //for each process
      for (unsigned int i=0; i<wNumProcesses; i++)
      {
        DWORD wPid = wProcessIds[i];

        processes.push_back(wPid);
      }
#else
      //Not implemented
#endif

      return processes;
    }

    processid_t getCurrentProcessId()
    {
#ifdef _WIN32
      processid_t pid = GetCurrentProcessId();
#else
      processid_t pid = getpid();
#endif
      return pid;
    }

    std::string getCurrentProcessDir()
    {
      std::string dir;
      std::string execPath = getCurrentProcessPath();
      if (execPath.empty())
        return dir; //failure
      dir = ra::filesystem::getParentPath(execPath);
      return dir;
    }

    processid_t startProcess(const std::string & iCommand, const std::string & iDefaultDirectory)
    {
    #ifdef _WIN32
      PROCESS_INFORMATION processInfo = {0};
    
      STARTUPINFO startupInfo = {0};
      startupInfo.cb = sizeof(STARTUPINFO);
      startupInfo.dwFlags = STARTF_USESHOWWINDOW;
      startupInfo.wShowWindow = SW_SHOWDEFAULT; //SW_SHOW, SW_SHOWNORMAL

      DWORD creationFlags = 0; //EXTENDED_STARTUPINFO_PRESENT

      bool success = (CreateProcess(NULL, (char*)iCommand.c_str(), NULL, NULL, FALSE, creationFlags, NULL, iDefaultDirectory.c_str(), &startupInfo, &processInfo) != 0);
      if (success)
      {
        //Wait for the application to initialize properly
        WaitForInputIdle(processInfo.hProcess, INFINITE);

        //Extract the program id
        DWORD dwProcessId = processInfo.dwProcessId;
        processid_t pId = static_cast<processid_t>(dwProcessId);
        return pId;
      }
      return INVALID_PROCESS_ID;
    #else
      //Not implemented yet
      return INVALID_PROCESS_ID;
    #endif
    }

    processid_t startProcess(const std::string & iExecPath, const std::string & iArguments, const std::string & iDefaultDirectory)
    {
      //merge iExecPath with iArguments
      std::string command;

      //handle iExecPath
      if (!iExecPath.empty())
      {
        if (iExecPath.find(" ") != std::string::npos)
        {
          command += "\"";
          command += iExecPath;
          command += "\"";
        }
        else
          command += iExecPath;
      }

      if (!iArguments.empty())
      {
        command += " ";
        command += iArguments;
      }

      if (command.size() > 0)
      {
        return startProcess(command, iDefaultDirectory);
      }

      return INVALID_PROCESS_ID;
    }

    bool openDocument(const std::string & iPath)
    {
    #ifdef _WIN32
      SHELLEXECUTEINFO info = {0};

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
      if (success)
      {
        HANDLE hProcess = info.hProcess;
        DWORD wPid = GetProcessId(hProcess);
        return true;
      }
      return false;
    #else
      //Not implemented yet
      return false;
    #endif
    }

    bool kill(const processid_t & pid)
    {
      bool success = false;
    #ifdef _WIN32
      //Get a handle
      HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, pid );
      if (hProcess)
      {
        success = (TerminateProcess(hProcess, 255) != 0);
        CloseHandle(hProcess);
      }
    #else
      //Not implemented yet
    #endif
      return success;
    }

#ifdef _WIN32
    /// <summary>
    /// Get the list of threads of a process.
    /// </summary>
    /// <param name="pid">The process id of the process.</param>
    /// <param name="tids">The list of thread ids of the process.</param>
    /// <returns>Returns true if the list of thread is returned. Returns false otherwise.</returns>
    bool getThreadIds(const processid_t & pid, ProcessIdList & tids)
    {
      tids.clear();

      //Getting threads id of the process
      HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
      THREADENTRY32 threadEntry;
   
      // Take a snapshot of all running threads
      hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
      if( hThreadSnap == INVALID_HANDLE_VALUE )
        return false;
   
      // Fill in the size of the structure before using it.
      threadEntry.dwSize = sizeof(THREADENTRY32);
   
      // Retrieve information about the first thread,
      // and exit if unsuccessful
      if( !Thread32First( hThreadSnap, &threadEntry ) )
      {
        CloseHandle( hThreadSnap ); // clean the snapshot object
        return false;
      }

      // Now walk the thread list of the system,
      // and display information about each thread
      // associated with the specified process
      do
      {
        if( threadEntry.th32OwnerProcessID == pid )
        {
          //printf( "\n\n     THREAD ID      = 0x%08X", threadEntry.th32ThreadID );
          //printf( "\n     Base priority  = %d", threadEntry.tpBasePri );
          //printf( "\n     Delta priority = %d", threadEntry.tpDeltaPri );
          tids.push_back(threadEntry.th32ThreadID);
        }
      } while( Thread32Next(hThreadSnap, &threadEntry ) );

      return true;
    }

    enum ExitCodeResult
    {
      EXIT_CODE_SUCCESS,
      EXIT_CODE_STILLRUNNING,
      EXIT_CODE_FAILED
    };

    ExitCodeResult getExitCode(const processid_t & pid, DWORD & code)
    {
      ExitCodeResult result = EXIT_CODE_FAILED;

      //Get a handle
      HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
      if (hProcess)
      {
        DWORD dwExitCode = 0;
        if(::GetExitCodeProcess(hProcess, &dwExitCode))
        {
          if(dwExitCode != STILL_ACTIVE)
          {
            result = EXIT_CODE_SUCCESS;
          }
          else
          {
            //Check if process is still alive
            DWORD dwR = ::WaitForSingleObject(hProcess, 0);
            if(dwR == WAIT_OBJECT_0)
            {
              result = EXIT_CODE_SUCCESS;
            }
            else if(dwR == WAIT_TIMEOUT)
            {
              result = EXIT_CODE_STILLRUNNING;
            }
            else
            {
              //Error
              result = EXIT_CODE_FAILED;
            }
          }
        }

        CloseHandle(hProcess);

        bool success = (result == EXIT_CODE_SUCCESS);
        if (success)
          code = dwExitCode;
      }
      return result;
    }

    typedef std::vector<HWND> HwndList;

    struct FindProcessWindowsStruct
    {
      HwndList * windowsPtr;
      processid_t pid;
    };

    BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
    {
      DWORD dwProcessId = 0;

      if (!hWnd)
        return TRUE;		// Not a window
      if (lParam == NULL)
        return TRUE;    // No FindProcessWindowsStruct pointer provided

      FindProcessWindowsStruct & s = (*((FindProcessWindowsStruct*)lParam));

      HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
      if (hInstance)
      {
        DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
        if (dwThreadId)
        {
          HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
          if (hProcess)
          {
            //is this the process we are looking for ?
            if (dwProcessId == s.pid)
            {
              //add found window handle to list
              s.windowsPtr->push_back(hWnd);
            }
          }
          CloseHandle(hProcess);
        }
      }
      return TRUE;
    }

    bool findProcessWindows(const processid_t & pid, HwndList & oWindows)
    {
      oWindows.clear();

      FindProcessWindowsStruct s;
      s.windowsPtr = &oWindows;
      s.pid = pid;

      bool success = (EnumWindows(EnumWindowsProc, (LPARAM)&s) == TRUE);
      return success;
    }

    bool closeWindows(const processid_t & pid)
    {
      HwndList hWnds;
      bool success = findProcessWindows(pid, hWnds);
      if (success)
      {
        for(size_t i=0; i<hWnds.size(); i++)
        {
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

    bool terminate(const processid_t & pid, DWORD iTimeoutMS)
    {
      bool success = false;

      //Get a handle
      HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
      if (hProcess)
      {
        ProcessIdList threadIds;
        if (getThreadIds(pid, threadIds))
        {
          size_t numThreads = threadIds.size();
          if (numThreads >= 1)
          {
            if (iTimeoutMS != INFINITE)
            {

              //Call WM_CLOSE & WM_QUIT on all the threads
              size_t threadTimeoutMS = iTimeoutMS/numThreads;
              for(size_t threadIndex = 0; threadIndex<numThreads && !success; threadIndex++)
              {
                DWORD threadId = threadIds[threadIndex];
                bool postSuccess = (PostThreadMessage(threadId, WM_CLOSE, 0, 0) != 0); //WM_CLOSE does not always work
                postSuccess = postSuccess && (PostThreadMessage(threadId, WM_QUIT, 0, 0) != 0);
                if (postSuccess)
                {
                  DWORD waitReturnCode = WaitForSingleObject(hProcess, threadTimeoutMS);
                  success = (waitReturnCode == WAIT_OBJECT_0);
                
                  //Some app does not signal the thread that accepted the WM_CLOSE or WM_QUIT messages
                  if (!success)
                    success = !isRunning(pid);

                  //Some app needs to have their windows closed
                  closeWindows(pid);
                }
              }
            }
            else
            {
              //Call WM_CLOSE & WM_QUIT on all the threads
              while(!success)
              {
                for(size_t threadIndex = 0; threadIndex<numThreads && !success; threadIndex++)
                {
                  DWORD threadId = threadIds[threadIndex];
                  bool postSuccess = (PostThreadMessage(threadId, WM_CLOSE, 0, 0) != 0); //WM_CLOSE does not always work
                  postSuccess = postSuccess && (PostThreadMessage(threadId, WM_QUIT, 0, 0) != 0);
                  if (postSuccess)
                  {
                    DWORD waitReturnCode = WaitForSingleObject(hProcess, 200);
                    success = (waitReturnCode == WAIT_OBJECT_0);
                  
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

#endif

    bool isRunning(const processid_t & pid)
    {
#ifdef _WIN32
      DWORD code;
      ExitCodeResult result = getExitCode(pid, code);
      bool alive = false;
      switch(result)
      {
      case EXIT_CODE_SUCCESS:
        alive = false;
        break;
      case EXIT_CODE_STILLRUNNING:
        alive = true;
        break;
      case EXIT_CODE_FAILED:
        {
          //Process p = findProcessByPid(pid);
          //alive = (p != Process::InvalidProcess);
          alive = true;
        }
        break;
      };
      return alive;
#else
      //Not Implemented
      return false;
#endif
    }

    bool terminate(const processid_t & pid)
    {
    #ifdef _WIN32
      bool terminated = terminate(pid, 30000); //allow 30 seconds to close
      return terminated;
    #else
      //Not implemented yet
      return false;
    #endif
    }

  } //namespace process
} //namespace ra
