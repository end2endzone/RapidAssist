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

#ifndef RA_PROCESS_UTF8_H
#define RA_PROCESS_UTF8_H

#include <string>
#include <vector>

#ifdef _WIN32
#include <stdint.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "rapidassist/config.h"
#include "rapidassist/strings.h"
#include "rapidassist/process.h"

namespace ra { namespace process {

#ifdef _WIN32 // UTF-8

  /// <summary>
  /// Provides the file path of the current executing process.
  /// </summary>
  /// <returns>Returns the file path of the current process. Returns an empty string on error.</returns>
  std::string GetCurrentProcessPathUtf8();

  /// <summary>
  /// Provides the directory of the current executing process.
  /// </summary>
  /// <returns>Returns the directory path of the current process. Returns an empty string on error.</returns>
  std::string GetCurrentProcessDirUtf8();

  /// <summary>
  /// Start the given process.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcessUtf8(const std::string & exec_path);

  /// <summary>
  /// Start the given process from the given directory.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <param name="default_directory">The directory to run the command from.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory);

  /// <summary>
  /// Start the given process with the given arguments from the given directory.
  /// Note: this api is only available on Windows.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <param name="default_directory">The directory to run the command from.</param>
  /// <param name="command_line">The command line to send to the new process.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory, const std::string & command_line);

  /// <summary>
  /// Open a document with the default system application.
  /// </summary>
  /// <param name="path">The path to the document to open.</param>
  /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
  bool OpenDocumentUtf8(const std::string & path);

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Provides the file path of the current executing process.
  /// </summary>
  /// <returns>Returns the file path of the current process. Returns an empty string on error.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetCurrentProcessPathUtf8() { return GetCurrentProcessPath(); }

  /// <summary>
  /// Provides the directory of the current executing process.
  /// </summary>
  /// <returns>Returns the directory path of the current process. Returns an empty string on error.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetCurrentProcessDirUtf8() { return GetCurrentProcessDir(); }

  /// <summary>
  /// Start the given process.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  inline processid_t StartProcessUtf8(const std::string & exec_path) { return StartProcess(exec_path); }

  /// <summary>
  /// Start the given process from the given directory.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <param name="default_directory">The directory to run the command from.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  inline processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory) { return StartProcess(exec_path, default_directory); }

  /// <summary>
  /// Start the given process with the given arguments from the given directory.
  /// Note: this api is only available on linux.
  /// </summary>
  /// <param name="exec_path">The path to the executable to start.</param>
  /// <param name="default_directory">The directory to run the command from.</param>
  /// <param name="arguments">The list of arguments for the new process.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  inline processid_t StartProcessUtf8(const std::string & exec_path, const std::string & default_directory, const ra::strings::StringVector & arguments) { return StartProcess(exec_path, default_directory, arguments); }

  /// <summary>
  /// Open a document with the default system application.
  /// </summary>
  /// <param name="path">The path to the document to open.</param>
  /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
  inline bool OpenDocumentUtf8(const std::string & path) { return OpenDocument(path); }

#endif // UTF-8

} //namespace process
} //namespace ra

#endif //RA_PROCESS_H
