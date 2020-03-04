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

#include <string>

#ifdef WIN32
//#   ifndef WIN32_LEAN_AND_MEAN
//#   define WIN32_LEAN_AND_MEAN 1
//#   endif
#   include <windows.h> // for GetModuleHandleEx()
#   undef GetCurrentDirectory
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

#endif // UTF-8

} //namespace process
} //namespace ra
