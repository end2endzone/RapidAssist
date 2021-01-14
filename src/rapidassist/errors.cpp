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

#include "rapidassist/errors.h"
#include "rapidassist/strings.h"

#ifdef _WIN32
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#else
#include <errno.h>    // for errno
#include <cerrno>     // for errno
#include <string.h>   // for strerror()
#endif

namespace ra { namespace errors {

  void ResetLastErrorCode() {
#ifdef _WIN32
    static DWORD last_error = 0;
    SetLastError(last_error);
#else
    errno = 0;
#endif
  }

  errorcode_t GetLastErrorCode() {
#ifdef _WIN32
    DWORD last_error = ::GetLastError();
    errorcode_t errcode = static_cast<errorcode_t>(last_error);
    return errcode;
#else
    int error_number = errno;
    errorcode_t errcode = static_cast<errorcode_t>(error_number);
    return errcode;
#endif
  }

  std::string GetLastErrorDescription() {
    errorcode_t code = GetLastErrorCode();
    std::string desc = GetErrorCodeDescription(code);
    return desc;
  }

  std::string GetErrorCodeDescription(errorcode_t code) {
#ifdef _WIN32
    DWORD last_error = static_cast<DWORD>(code);
    const DWORD error_buffer_size = 10240;
    char error_buffer[error_buffer_size] = { 0 };
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      last_error,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      error_buffer,
      error_buffer_size - 1,
      NULL);
    ra::strings::RemoveEol(error_buffer); //error message have a CRLF at the end.
    std::string error_desc = error_buffer;
    return error_desc;
#else
    int error_number = static_cast<int>(code);
    std::string desc = strerror(error_number);
    return desc;
#endif
  }

} //namespace errors
} //namespace ra
