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
#else
#include <errno.h>    // for errno
#include <cerrno>     // for errno
#include <string.h>   // for strerror()
#endif

namespace ra
{
  namespace errors
  {

    void resetLastErrorCode()
    {
      #ifdef _WIN32
      static DWORD dwLastError = 0;
      SetLastError(dwLastError);
      #else
      errno = 0;
      #endif
    }

    int32_t getLastErrorCode()
    {
      #ifdef _WIN32
      DWORD dwLastError = ::GetLastError();
      int32_t errcode = static_cast<int32_t>(dwLastError);
      return errcode;
      #else
      int errnum = errno;
      int32_t errcode = static_cast<int32_t>(errnum);
      return errcode;
      #endif
    }

    std::string getLastErrorDescription()
    {
      int32_t errcode = getLastErrorCode();
      std::string desc = getErrorCodeDescription(errcode);
      return desc;
    }

    std::string getErrorCodeDescription(int32_t errcode)
    {
      #ifdef _WIN32
      DWORD dwLastError = static_cast<DWORD>(errcode);
      const DWORD dwErrorBufferSize = 10240;
      char lpErrorBuffer[dwErrorBufferSize] = {0};
      ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      dwLastError,
                      MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                      lpErrorBuffer,
                      dwErrorBufferSize-1,
                      NULL);
      ra::strings::removeEOL(lpErrorBuffer); //error message have a CRLF at the end.
      std::string strError = lpErrorBuffer;
      return strError;
      #else
      int errnum = static_cast<int>(errcode);
      std::string desc = strerror(errnum);
      return desc;
      #endif
    }

  } //namespace errors
} //namespace ra
