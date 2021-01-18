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

#ifndef RA_ENVIRONMENT_H
#define RA_ENVIRONMENT_H

#include <string>
#include <stdint.h>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"

namespace ra { namespace environment {

  /// <summary>
  /// Returns the current value of an environment variable.
  /// </summary>
  /// <param name="name">Name of the variable</param>
  /// <returns>Returns the value of the given environment variable.</returns>
  std::string GetEnvironmentVariable(const char * name);

  /// <summary>
  /// Sets a new value for the given environment variable.
  /// </summary>
  /// <remarks>Use SetEnvironmentVariable(name, NULL) for deleting an existing variable.</remarks>
  /// <param name="name">Name of the variable</param>
  /// <param name="value">New value of the variable. Use NULL for deleting a variable.</param>
  /// <returns>Returns true when successful. Returns false otherwise.</returns>
  bool SetEnvironmentVariable(const char * name, const     char * value);
  bool SetEnvironmentVariable(const char * name, const   int8_t & value);
  bool SetEnvironmentVariable(const char * name, const  uint8_t & value);
  bool SetEnvironmentVariable(const char * name, const  int16_t & value);
  bool SetEnvironmentVariable(const char * name, const uint16_t & value);
  bool SetEnvironmentVariable(const char * name, const  int32_t & value);
  bool SetEnvironmentVariable(const char * name, const uint32_t & value);
  bool SetEnvironmentVariable(const char * name, const  int64_t & value);
  bool SetEnvironmentVariable(const char * name, const uint64_t & value);
  bool SetEnvironmentVariable(const char * name, const    float & value);
  bool SetEnvironmentVariable(const char * name, const   double & value);

  /// <summary>
  /// Returns true if the current process is 32 bit.
  /// </summary>
  /// <returns>Returns true if the current process is 32 bit. Returns false otherwise.</returns>
  bool IsProcess32Bit();

  /// <summary>
  /// Returns true if the current process is 64 bit.
  /// </summary>
  /// <returns>Returns true if the current process is 64 bit. Returns false otherwise.</returns>
  bool IsProcess64Bit();

  /// <summary>
  /// Returns true if the current process is in Debug mode.
  /// </summary>
  /// <returns>Returns true if the current process is in Debug mode. Returns false otherwise.</returns>
  inline bool IsConfigurationDebug() {
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
  }

  /// <summary>
  /// Returns true if the current process is in Release mode.
  /// </summary>
  /// <returns>Returns true if the current process is in Release mode. Returns false otherwise.</returns>
  inline bool IsConfigurationRelease() {
#ifdef NDEBUG
    return true;
#else
    return false;
#endif
  }

  /// <summary>
  /// Returns the line separator of the system.
  /// </summary>
  /// <returns>Returns the line separator for the current system.</returns>
  const char * GetLineSeparator();

  /// <summary>
  /// Returns the list of all environment variables defined by the current process.
  /// </summary>
  /// <returns>Returns a list of all environment variables.</returns>
  ra::strings::StringVector GetEnvironmentVariables();

  /// <summary>
  /// Expand a file path by replacing environment variable reference by the actual variable's value.
  /// Unix    environment variables syntax must in the following form:  $name where  'name' is an environment variable.
  /// Windows environment variables syntax must in the following form:  %name% where 'name' is an environment variable.
  /// </summary>
  /// <param name="value">The path that must be expanded.</param>
  /// <returns>Returns a new string with the expanded strings.</returns>
  std::string Expand(const std::string & value);

} //namespace environment
} //namespace ra

#endif //RA_ENVIRONMENT_H
