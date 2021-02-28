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

#ifndef RA_ENVIRONMENT_UTF8_H
#define RA_ENVIRONMENT_UTF8_H

#include <stdint.h>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"
#include "rapidassist/environment.h"

namespace ra { namespace environment {

#ifdef _WIN32 // UTF-8
  /// <summary>
  /// Returns the current value of an environment variable.
  /// </summary>
  /// <param name="name">Name of the variable</param>
  /// <returns>Returns the value of the given environment variable.</returns>
  std::string GetEnvironmentVariableUtf8(const char * name);

  /// <summary>
  /// Sets a new utf-8 encoded value for the given environment variable.
  /// </summary>
  /// <remarks>Use SetEnvironmentVariableUtf8(name, NULL) for deleting an existing variable.</remarks>
  /// <param name="name">Name of the variable</param>
  /// <param name="value">New value of the variable. Use NULL for deleting a variable.</param>
  /// <returns>Returns true when successful. Returns false otherwise.</returns>
  bool SetEnvironmentVariableUtf8(const char * name, const   char *   value);

  /// <summary>
  /// Returns the list of all environment variables defined by the current process.
  /// </summary>
  /// <returns>Returns a list of all environment variables.</returns>
  ra::strings::StringVector GetEnvironmentVariablesUtf8();

  /// <summary>
  /// Expand a file path by replacing environment variable reference by the actual variable's value.
  /// Unix    environment variables syntax must in the following form:  $name where  'name' is an environment variable.
  /// Windows environment variables syntax must in the following form:  %name% where 'name' is an environment variable.
  /// </summary>
  /// <param name="value">The path that must be expanded.</param>
  /// <returns>Returns a new string with the expanded strings.</returns>
  std::string ExpandUtf8(const std::string & value);

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Returns the current value of an environment variable.
  /// </summary>
  /// <param name="name">Name of the variable</param>
  /// <returns>Returns the value of the given environment variable.</returns>
  /// <remarks>On Linux, this function delegates to GetEnvironmentVariable(). It provides cross-platform compatibility for Windows users.</remarks>
  inline std::string GetEnvironmentVariableUtf8(const char * name) { return GetEnvironmentVariable(name); }

  /// <summary>
  /// Sets a new utf-8 encoded value for the given environment variable.
  /// </summary>
  /// <remarks>Use SetEnvironmentVariableUtf8(name, NULL) for deleting an existing variable.</remarks>
  /// <param name="name">Name of the variable</param>
  /// <param name="value">New value of the variable. Use NULL for deleting a variable.</param>
  /// <returns>Returns true when successful. Returns false otherwise.</returns>
  /// <remarks>On Linux, this function delegates to SetEnvironmentVariable(). It provides cross-platform compatibility for Windows users.</remarks>
  inline bool SetEnvironmentVariableUtf8(const char * name, const   char *   value) { return SetEnvironmentVariable(name, value); }

  /// <summary>
  /// Returns the list of all environment variables defined by the current process.
  /// </summary>
  /// <returns>Returns a list of all environment variables.</returns>
  /// <remarks>On Linux, this function delegates to GetEnvironmentVariables(). It provides cross-platform compatibility for Windows users.</remarks>
  inline ra::strings::StringVector GetEnvironmentVariablesUtf8() { return GetEnvironmentVariables(); }

  /// <summary>
  /// Expand a file path by replacing environment variable reference by the actual variable's value.
  /// Unix    environment variables syntax must in the following form:  $name where  'name' is an environment variable.
  /// Windows environment variables syntax must in the following form:  %name% where 'name' is an environment variable.
  /// </summary>
  /// <param name="value">The path that must be expanded.</param>
  /// <returns>Returns a new string with the expanded strings.</returns>
  /// <remarks>On Linux, this function delegates to Expand(). It provides cross-platform compatibility for Windows users.</remarks>
  inline std::string ExpandUtf8(const std::string & value) { return Expand(value); }

#endif // UTF-8

} //namespace environment
} //namespace ra

#endif //RA_ENVIRONMENT_UTF8_H
