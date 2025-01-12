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

#ifndef RA_ERRORS_UTF8_H
#define RA_ERRORS_UTF8_H

#include <stdint.h>

#include "rapidassist/config.h"
#include "rapidassist/errors.h"

namespace ra { namespace errors {

#ifdef _WIN32 // UTF-8

  /// <summary>
  /// Returns the description of the last error.
  /// </summary>
  /// <returns>Returns the description of the last error.</returns>
  std::string GetLastErrorDescriptionUtf8();

  /// <summary>
  /// Returns the description of the given error code.
  /// </summary>
  /// <returns>Returns the description of the given error code.</returns>
  std::string GetErrorCodeDescriptionUtf8(errorcode_t code);

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Returns the description of the last error.
  /// </summary>
  /// <returns>Returns the description of the last error.</returns>
  inline std::string GetLastErrorDescriptionUtf8() { return GetLastErrorDescription(); }

  /// <summary>
  /// Returns the description of the given error code.
  /// </summary>
  /// <returns>Returns the description of the given error code.</returns>
  inline std::string GetErrorCodeDescriptionUtf8(errorcode_t code) { return GetErrorCodeDescription(code); }

#endif // UTF-8

} //namespace environment
} //namespace ra

#endif //RA_ERRORS_UTF8_H
