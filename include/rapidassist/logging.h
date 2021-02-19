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

#ifndef RA_LOGGING_H
#define RA_LOGGING_H

#include <string>

#include "rapidassist/config.h"

namespace ra { namespace logging {

  /// <summary>
  /// Different logging levels
  /// </summary>
  enum LoggerLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
  };

  /// <summary>
  /// Sets the quiet mode enabled or disabled.
  /// Silences all log of level LOG_INFO 
  /// </summary>
  /// <param name="quiet">The new value of the quiet mode.</param>
  void SetQuietMode(bool quiet);

  /// <summary>
  /// Returns true if the quiet mode is enabled.
  /// </summary>
  /// <returns>Returns true if the quiet mode is enabled.</returns>
  bool IsQuietModeEnabled();

  /// <summary>
  /// Prints the given arguments to the console depending on the specified logging level.
  /// </summary>
  /// <param name="level">The level of the given arguments</param>
  /// <param name="format">The format of the given argument. Same as printf's format.</param>
  void Log(LoggerLevel level, const char * format, ...);

} //namespace logging
} //namespace ra

#endif //RA_LOGGING_H