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

#include "rapidassist/user.h"
#include "rapidassist/environment.h"

#ifdef WIN32
#   include <Shlobj.h>
#   include <windows.h>
#   undef GetEnvironmentVariable
#   include <Lmcons.h>
#else
#   include <stdlib.h>
#   include <pwd.h>
#   include <stdio.h>
#   include <unistd.h>
#   include <sys/types.h>
#endif

namespace ra { namespace user {

#ifdef _WIN32
  inline std::string getWin32Directory(int csidl) {
    // https://stackoverflow.com/questions/18493484/shgetfolderpath-deprecated-what-is-alternative-to-retrieve-path-for-windows-fol
    // https://superuser.com/questions/150012/what-is-the-difference-between-local-and-roaming-folders
    // CSIDL_PROFILE          matches "C:\Users\JohnSmith"
    // CSIDL_PERSONAL         matches "C:\Users\JohnSmith\Documents"
    // CSIDL_APPDATA          matches "C:\Users\JohnSmith\AppData\Roaming"
    // CSIDL_LOCAL_APPDATA    matches "C:\Users\JohnSmith\AppData\Local"
    // CSIDL_COMMON_APPDATA   matches "C:\ProgramData"
    // CSIDL_COMMON_DOCUMENTS matches "C:\Users\Public\Documents"
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetSpecialFolderPath(NULL, path, csidl, FALSE))) {
      return path;
    }
    return "";
  }
#endif

  std::string getHomeDirectory() {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_PROFILE);
    return dir;
#else
    //https://stackoverflow.com/questions/1610203/unix-programming-not-sure-how-to-use-the-passwd-struct
    //https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
    struct passwd pwd;
    struct passwd *result = NULL;
    char buf[1024];
    uid_t uid = geteuid();
    if (getpwuid_r(uid, &pwd, buf, sizeof(buf), &result) == 0) {
      if (result != NULL)
        return result->pw_dir;
    }

    //fallback to HOME env variable
    std::string env_home = ra::environment::GetEnvironmentVariable("HOME");
    if (!env_home.empty())
      return env_home;

    //failure
    return "~";
#endif
  }

  std::string getApplicationsDataDirectory() {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_LOCAL_APPDATA);
    return dir;
#else
    return "/usr/share";
#endif
  }

  std::string getDocumentsDirectory() {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_PERSONAL);
    return dir;
#else
    return std::string(getHomeDirectory()) + "/Documents";
#endif
  }

  std::string getDesktopDirectory() {
#ifdef _WIN32
    std::string dir = getWin32Directory(CSIDL_DESKTOPDIRECTORY);
    return dir;
#else
    return std::string(getHomeDirectory()) + "/Desktop";
#endif
  }

  std::string getUsername() {
#ifdef _WIN32
    char username[UNLEN + 1] = { 0 };
    DWORD size = UNLEN + 1;
    if (SUCCEEDED(GetUserName(username, &size))) {
      return username;
    }

    //fallback to USERNAME env variable
    std::string env_username = ra::environment::GetEnvironmentVariable("USERNAME");
    if (!env_username.empty())
      return env_username;

    //failure
    return "";
#else
    //https://stackoverflow.com/questions/1610203/unix-programming-not-sure-how-to-use-the-passwd-struct
    //https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
    struct passwd pwd;
    struct passwd *result = NULL;
    char buf[1024];
    uid_t uid = geteuid();
    if (getpwuid_r(uid, &pwd, buf, sizeof(buf), &result) == 0) {
      if (result != NULL)
        return result->pw_name;
    }

    //fallback to LOGNAME env variable
    std::string env_logname = ra::environment::GetEnvironmentVariable("LOGNAME");
    if (!env_logname.empty())
      return env_logname;

    //failure
    return "";
#endif
  }

} //namespace user
} //namespace ra
