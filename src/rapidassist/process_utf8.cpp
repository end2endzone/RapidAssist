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

#include "rapidassist/process_utf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/timing.h"
#include "rapidassist/unicode.h"

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

namespace ra { namespace process {

#ifdef _WIN32 // UTF-8

  std::string GetCurrentProcessPathUtf8() {
    std::string path;

    HMODULE hModule = GetModuleHandleW(NULL);
    if (hModule == NULL) {
      int ret = GetLastError();
      return path; //failure
    }
    //get the path of this process
    wchar_t buffer[MAX_PATH] = { 0 };
    if (!GetModuleFileNameW(hModule, buffer, sizeof(buffer))) {
      int ret = GetLastError();
      return path; //failure
    }

    std::wstring pathW = buffer;
    path = ra::unicode::UnicodeToUtf8(pathW);

    return path;
  }

  std::string GetCurrentProcessDirUtf8() {
    std::string dir;
    std::string exec_path = GetCurrentProcessPathUtf8();
    if (exec_path.empty())
      return dir; //failure
    dir = ra::filesystem::GetParentPath(exec_path);
    return dir;
  }

  processid_t StartProcessUtf8(const std::string & exec_path) {
    std::string curr_dir = ra::filesystem::GetCurrentDirectoryUtf8();

    // Launch the process from the current process current directory
    processid_t pid = StartProcessUtf8(exec_path, curr_dir);
    return pid;
  }

  processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory) {
    // Launch the process with no arguments
    processid_t pid = StartProcessUtf8(exec_path, default_directory, "");
    return pid;
  }

  processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory, const std::string & command_line) {
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

    const std::wstring commandW = ra::unicode::Utf8ToUnicode(command);
    const std::wstring defaultDirectoryW = ra::unicode::Utf8ToUnicode(default_directory);

    //launch a new process with the command line
    PROCESS_INFORMATION process_info = { 0 };
    STARTUPINFOW startup_info = { 0 };
    startup_info.cb = sizeof(STARTUPINFOW);
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOWDEFAULT; //SW_SHOW, SW_SHOWNORMAL
    static const DWORD creation_flags = 0; //EXTENDED_STARTUPINFO_PRESENT
    bool success = (CreateProcessW(NULL, (wchar_t*)commandW.c_str(), NULL, NULL, FALSE, creation_flags, NULL, defaultDirectoryW.c_str(), &startup_info, &process_info) != 0);
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

  bool OpenDocumentUtf8(const std::string & path) {
    if (!ra::filesystem::FileExistsUtf8(path.c_str()))
      return false; //file not found

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    SHELLEXECUTEINFOW info = { 0 };

    info.cbSize = sizeof(SHELLEXECUTEINFOW);

    info.fMask |= SEE_MASK_NOCLOSEPROCESS;
    info.fMask |= SEE_MASK_NOASYNC;
    info.fMask |= SEE_MASK_FLAG_DDEWAIT;

    info.hwnd = HWND_DESKTOP;
    info.nShow = SW_SHOWDEFAULT;
    info.lpVerb = L"open";
    info.lpFile = pathW.c_str();
    info.lpParameters = NULL; //arguments
    info.lpDirectory = NULL; // default directory

    BOOL success = ShellExecuteExW(&info);
    if (success) {
      HANDLE hProcess = info.hProcess;
      DWORD pid = GetProcessId(hProcess);
      return true;
    }
    return false;
  }

#endif // UTF-8

} //namespace process
} //namespace ra
