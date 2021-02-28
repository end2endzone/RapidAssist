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

#ifndef RA_USER_UTF8_H
#define RA_USER_UTF8_H

#include <string>

#include "rapidassist/config.h"
#include "rapidassist/user.h"

namespace ra { namespace user {

#ifdef _WIN32 // UTF-8

  /// <summary>
  /// Returns the current user 'home' directory.
  /// For unix    systems, the returned path is "~".
  /// For windows systems, the returned path is "%USERPROFILE%" (ie "C:\Users\JohnSmith").
  /// </summary>
  /// <returns>Returns the current user's home directory.</returns>
  std::string GetHomeDirectoryUtf8();

  /// <summary>
  /// Returns the current user 'application data' directory.
  /// For unix    systems, the returned path is "/usr/share".
  /// For windows systems, the returned path is "%USERPROFILE%\AppData\Local" (ie "C:\Users\JohnSmith\AppData\Local").
  /// </summary>
  /// <returns>Returns the current user's application data directory.</returns>
  std::string GetApplicationsDataDirectoryUtf8();

  /// <summary>
  /// Returns the current user 'documents' directory.
  /// For unix    systems, the returned path is "~/Documents".
  /// For windows systems, the returned path is "%USERPROFILE%\Documents" (ie "C:\Users\JohnSmith\Documents").
  /// </summary>
  /// <returns>Returns the current user's documents directory.</returns>
  std::string GetDocumentsDirectoryUtf8();

  /// <summary>
  /// Returns the current user 'desktop' directory.
  /// For unix    systems, the returned path is "~/Desktop".
  /// For windows systems, the returned path is "%USERPROFILE%\Desktop" (ie "C:\Users\JohnSmith\Desktop").
  /// </summary>
  /// <returns>Returns the current user's desktop directory.</returns>
  std::string GetDesktopDirectoryUtf8();

  /// <summary>
  /// Returns the user's login username.
  /// </summary>
  /// <returns>Returns the user's login username.</returns>
  std::string GetUsernameUtf8();

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Returns the current user 'home' directory.
  /// For unix    systems, the returned path is "~".
  /// For windows systems, the returned path is "%USERPROFILE%" (ie "C:\Users\JohnSmith").
  /// </summary>
  /// <returns>Returns the current user's home directory.</returns>
  inline std::string GetHomeDirectoryUtf8() { return GetHomeDirectory(); }

  /// <summary>
  /// Returns the current user 'application data' directory.
  /// For unix    systems, the returned path is "/usr/share".
  /// For windows systems, the returned path is "%USERPROFILE%\AppData\Local" (ie "C:\Users\JohnSmith\AppData\Local").
  /// </summary>
  /// <returns>Returns the current user's application data directory.</returns>
  inline std::string GetApplicationsDataDirectoryUtf8() { return GetApplicationsDataDirectory(); }

  /// <summary>
  /// Returns the current user 'documents' directory.
  /// For unix    systems, the returned path is "~/Documents".
  /// For windows systems, the returned path is "%USERPROFILE%\Documents" (ie "C:\Users\JohnSmith\Documents").
  /// </summary>
  /// <returns>Returns the current user's documents directory.</returns>
  inline std::string GetDocumentsDirectoryUtf8() { return GetDocumentsDirectory(); }

  /// <summary>
  /// Returns the current user 'desktop' directory.
  /// For unix    systems, the returned path is "~/Desktop".
  /// For windows systems, the returned path is "%USERPROFILE%\Desktop" (ie "C:\Users\JohnSmith\Desktop").
  /// </summary>
  /// <returns>Returns the current user's desktop directory.</returns>
  inline std::string GetDesktopDirectoryUtf8() { return GetDesktopDirectory(); }

  /// <summary>
  /// Returns the user's login username.
  /// </summary>
  /// <returns>Returns the user's login username.</returns>
  inline std::string GetUsernameUtf8() { return GetUsername(); }

#endif // UTF-8
} //namespace user
} //namespace ra

#endif //RA_USER_UTF8_H
