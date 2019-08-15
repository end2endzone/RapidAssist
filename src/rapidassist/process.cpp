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

  } //namespace process
} //namespace ra
