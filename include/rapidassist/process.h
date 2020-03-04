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

#ifndef RA_PROCESS_H
#define RA_PROCESS_H

#include <string>
#include <vector>

#ifdef _WIN32
#include <stdint.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "rapidassist/strings.h"
#include "rapidassist/process_utf8.h"

namespace ra { namespace process {

#ifdef _WIN32
  /// <summary>Defines a process id type for the WIN32 platform. The type is unsigned. The native type is DWORD which is the same as uint32_t.</summary>
  typedef uint32_t processid_t;
#else
  /// <summary>Defines a process id type for the linux platform. The type is   signed.</summary>
  typedef pid_t processid_t;
#endif

  /// <summary>Defines a list of processes id.</summary>
  typedef std::vector<processid_t> ProcessIdList;

  /// <summary>Defines an invalid processes id.</summary>
  extern const processid_t INVALID_PROCESS_ID;

  /// <summary>
  /// Converts a list of process id to a string.
  /// </summary>
  /// <returns>Returns a list of process id as a string.</returns>
  std::string ToString(const ProcessIdList & processes);

  /// <summary>
  /// Provides the file path of the current executing process.
  /// </summary>
  /// <returns>Returns the file path of the current process. Returns an empty string on error.</returns>
  std::string GetCurrentProcessPath();

  /// <summary>
  /// Provides the directory of the current executing process.
  /// </summary>
  /// <returns>Returns the directory path of the current process. Returns an empty string on error.</returns>
  std::string GetCurrentProcessDir();

  /// <summary>
  /// Get the list of all running processes of the system.
  /// </summary>
  /// <returns>Returns the list of all running processes of the system.</returns>
  ProcessIdList GetProcesses();

  /// <summary>
  /// Get the current process id.
  /// </summary>
  /// <returns>Returns the current process id.</returns>
  processid_t GetCurrentProcessId();

  /// <summary>
  /// Start the given process.
  /// </summary>
  /// <param name="iExecPath">The path to the executable to start.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcess(const std::string & iExecPath);

  /// <summary>
  /// Start the given process from the given directory.
  /// </summary>
  /// <param name="iExecPath">The path to the executable to start.</param>
  /// <param name="iDefaultDirectory">The directory to run the command from.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcess(const std::string & iExecPath, const std::string & iDefaultDirectory);

#ifdef _WIN32
  /// <summary>
  /// Start the given process with the given arguments from the given directory.
  /// Note: this api is only available on Windows.
  /// </summary>
  /// <param name="iExecPath">The path to the executable to start.</param>
  /// <param name="iDefaultDirectory">The directory to run the command from.</param>
  /// <param name="iCommandLine">The command line to send to the new process.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcess(const std::string & iExecPath, const std::string & iDefaultDirectory, const std::string & iCommandLine);
#else
  /// <summary>
  /// Start the given process with the given arguments from the given directory.
  /// Note: this api is only available on linux.
  /// </summary>
  /// <param name="iExecPath">The path to the executable to start.</param>
  /// <param name="iDefaultDirectory">The directory to run the command from.</param>
  /// <param name="iArguments">The list of arguments for the new process.</param>
  /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
  processid_t StartProcess(const std::string & iExecPath, const std::string & iDefaultDirectory, const ra::strings::StringVector & iArguments);
#endif

  /// <summary>
  /// Open a document with the default system application.
  /// </summary>
  /// <param name="iPath">The path to the document to open.</param>
  /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
  bool OpenDocument(const std::string & iPath);

  /// <summary>
  /// Kill an existing process using the process id.
  /// </summary>
  /// <param name="pid">The process id of the target process to kill.</param>
  /// <returns>Returns true if the process was successfully killed. Returns false otherwise.</returns>
  bool Kill(const processid_t & pid);

  /// <summary>
  /// Terminate gracefully an existing process using the process id.
  /// </summary>
  /// <param name="pid">The process id of the target process to Terminate.</param>
  /// <returns>Returns true if the process was successfully terminated. Returns false otherwise.</returns>
  bool Terminate(const processid_t & pid);

  /// <summary>
  /// Verify the given process id is valid.
  /// </summary>
  /// <param name="pid">The process id to verify.</param>
  /// <returns>Returns true if the process is alive. Returns false otherwise.</returns>
  bool IsRunning(const processid_t & pid);

  /// <summary>
  /// Returns the exit code of the given process id.
  /// The process must be terminated for the function to be successful.
  /// The process must be a child process of the current process for the function to be successful.
  /// </summary>
  /// <param name="pid">The process id to verify.</param>
  /// <param name="exitcode">The process exit code if the function is successful.</param>
  /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
  bool GetExitCode(const processid_t & pid, int & exitcode);

  /// <summary>
  /// Wait for the given process termination.
  /// The process must be a child process of the current process for the function to be successful.
  /// </summary>
  /// <param name="pid">The process id to wait for.</param>
  /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
  bool WaitExit(const processid_t & pid);

  /// <summary>
  /// Wait for the given process termination and return the process exit code.
  /// The process must be a child process of the current process for the function to be successful.
  /// </summary>
  /// <param name="pid">The process id to wait for.</param>
  /// <param name="exitcode">The process exit code if the function is successful.</param>
  /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
  bool WaitExit(const processid_t & pid, int & exitcode);

} //namespace process
} //namespace ra

#endif //RA_PROCESS_H
