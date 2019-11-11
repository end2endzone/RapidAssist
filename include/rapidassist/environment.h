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

#include "rapidassist/strings.h"

namespace ra { namespace environment {

  /// <summary>
  /// Returns value of an environment variable.
  /// </summary>
  /// <param name="iName">Name of the variable</param>
  /// <returns>Returns the value of the given environment variable.</returns>
  std::string GetEnvironmentVariable(const char * iName);

  /// <summary>
  /// Sets a new value for the given environment variable.
  /// </summary>
  /// <remarks>Use SetEnvironmentVariable(name, NULL) for deleting an existing variable.</remarks>
  /// <param name="iName">Name of the variable</param>
  /// <param name="iValue">New value of the variable. Use NULL for deleting a variable.</param>
  /// <returns>Returns true when successful. Returns false otherwise.</returns>
  bool SetEnvironmentVariable(const char * iName, const char * iValue);
  bool SetEnvironmentVariable(const char * iName, const   int8_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const  uint8_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const  int16_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const uint16_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const  int32_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const uint32_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const  int64_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const uint64_t & iValue);
  bool SetEnvironmentVariable(const char * iName, const    float & iValue);
  bool SetEnvironmentVariable(const char * iName, const   double & iValue);

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
  bool IsConfigurationDebug();

  /// <summary>
  /// Returns true if the current process is in Release mode.
  /// </summary>
  /// <returns>Returns true if the current process is in Release mode. Returns false otherwise.</returns>
  bool IsConfigurationRelease();

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
  /// <param name="iValue">The path that must be expanded.</param>
  /// <returns>Returns a new string with the expanded strings.</returns>
  std::string Expand(const std::string & iValue);

} //namespace environment
} //namespace ra

#endif //RA_ENVIRONMENT_H
