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

#include "rapidassist/user_utf8.h"
#include "rapidassist/environment_utf8.h"
#include "rapidassist/unicode.h"

#ifdef _WIN32
#   include <Shlobj.h>
#   include <Windows.h>
#   include "rapidassist/undef_windows_macros.h"
#   include <Lmcons.h>
#else
#   include <stdlib.h>
#   include <pwd.h>
#   include <stdio.h>
#   include <unistd.h>
#   include <sys/types.h>
#endif

namespace ra { namespace user {

#ifdef _WIN32 // UTF-8
  inline std::string GetWin32DirectoryUtf8(int csidl) {
    // https://stackoverflow.com/questions/18493484/shgetfolderpath-deprecated-what-is-alternative-to-retrieve-path-for-windows-fol
    // https://superuser.com/questions/150012/what-is-the-difference-between-local-and-roaming-folders
    // CSIDL_PROFILE          matches "C:\Users\JohnSmith"
    // CSIDL_PERSONAL         matches "C:\Users\JohnSmith\Documents"
    // CSIDL_APPDATA          matches "C:\Users\JohnSmith\AppData\Roaming"
    // CSIDL_LOCAL_APPDATA    matches "C:\Users\JohnSmith\AppData\Local"
    // CSIDL_COMMON_APPDATA   matches "C:\ProgramData"
    // CSIDL_COMMON_DOCUMENTS matches "C:\Users\Public\Documents"
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetSpecialFolderPathW(NULL, path, csidl, FALSE))) {
      std::string path_utf8 = ra::unicode::UnicodeToUtf8(path); //convert from Wide character (Unicode) to UTF-8
      return path_utf8;
    }
    return "";
  }

  std::string GetHomeDirectoryUtf8() {
    const std::string & dir = GetWin32DirectoryUtf8(CSIDL_PROFILE);
    return dir;
  }

  std::string GetApplicationsDataDirectoryUtf8() {
    const std::string & dir = GetWin32DirectoryUtf8(CSIDL_LOCAL_APPDATA);
    return dir;
  }

  std::string GetDocumentsDirectoryUtf8() {
    const std::string & dir = GetWin32DirectoryUtf8(CSIDL_PERSONAL);
    return dir;
  }

  std::string GetDesktopDirectoryUtf8() {
    const std::string & dir = GetWin32DirectoryUtf8(CSIDL_DESKTOPDIRECTORY);
    return dir;
  }

  std::string GetUsernameUtf8() {
    wchar_t username[UNLEN + 1] = { 0 };
    DWORD size = UNLEN + 1;
    if (SUCCEEDED(GetUserNameW(username, &size))) {
      std::string username_utf8 = ra::unicode::UnicodeToUtf8(username); //convert from Wide character (Unicode) to UTF-8
      return username_utf8;
    }

    //fallback to USERNAME env variable
    std::string env_username = ra::environment::GetEnvironmentVariableUtf8("USERNAME");
    if (!env_username.empty())
      return env_username;

    //failure
    return "";
  }

#endif // UTF-8

} //namespace user
} //namespace ra
