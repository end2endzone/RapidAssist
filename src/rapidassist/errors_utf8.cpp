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

#include "rapidassist/errors_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/strings.h"

#ifdef _WIN32
#include <Windows.h> //for CreateFile()
#include "rapidassist/undef_windows_macros.h"
#endif

namespace ra { namespace errors {

#ifdef _WIN32 // UTF-8

  std::string GetLastErrorDescriptionUtf8()
  {
    errorcode_t code = GetLastErrorCode();
    std::string desc = GetErrorCodeDescriptionUtf8(code);
    return desc;
  }

  std::string GetErrorCodeDescriptionUtf8(errorcode_t code)
  {
#ifdef _WIN32
    DWORD last_error = static_cast<DWORD>(code);
    const DWORD error_buffer_size = 10240;
    wchar_t error_buffer[error_buffer_size] = { 0 };
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      last_error,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      error_buffer,
      error_buffer_size - 1,
      NULL);
    std::wstring error_desc_unicode = error_buffer;
    std::string error_desc_utf8 = ra::unicode::UnicodeToUtf8(error_desc_unicode);
    ra::strings::RemoveEol(error_desc_utf8); //error message have a CRLF at the end.
    return error_desc_utf8;
#else
    int error_number = static_cast<int>(code);
    std::string desc = strerror(error_number);
    return desc;
#endif
  }

#endif // UTF-8

} //namespace testing
} //namespace ra
