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

#ifdef _WIN32
#include <stdint.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace ra
{
  namespace process
  {

    #ifdef _WIN32
    /// <summary>Defines a process id type for the WIN32 platform. The type is unsigned. The native type is DWORD which is the same as uint32_t.</summary>
    typedef uint32_t processid_t;
    #else
    /// <summary>Defines a process id type for the linux platform. The type is   signed.</summary>
    typedef pid_t processid_t;
    #endif

    extern const processid_t INVALID_PROCESS_ID;

    /// <summary>
    /// Provides the file path of the current executing process.
    /// </summary>
    /// <returns>Returns the file path of the current process. Returns an empty string on error.</returns>
    std::string getCurrentProcessPath();

    /// <summary>
    /// Provides the directory of the current executing process.
    /// </summary>
    /// <returns>Returns the directory path of the current process. Returns an empty string on error.</returns>
    std::string getCurrentProcessDir();

    /// <summary>
    /// Start the given process from the given directory.
    /// </summary>
    /// <param name="iCommand">The command to start. Can include a process path and/or arguments.</param>
    /// <param name="iDefaultDirectory">The directory to run the command from.</param>
    /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
    processid_t startProcess(const std::string & iCommand, const std::string & iDefaultDirectory);

    /// <summary>
    /// Start the given process with the given arguments from the given directory.
    /// </summary>
    /// <param name="iExecPath">The path to the executable to start.</param>
    /// <param name="iArguments">The arguments to send to the process.</param>
    /// <param name="iDefaultDirectory">The directory to run the command from.</param>
    /// <returns>Returns the process id when successful. Returns INVALID_PROCESS_ID otherwise.</returns>
    processid_t startProcess(const std::string & iExecPath, const std::string & iArguments, const std::string & iDefaultDirectory);

    /// <summary>
    /// Open a document with the default system application.
    /// </summary>
    /// <param name="iPath">The path to the document to open.</param>
    /// <returns>Returns true if the document was opened with the system's default application. Returns false otherwise.</returns>
    bool openDocument(const std::string & iPath);


  } //namespace process
} //namespace ra

#endif //RA_PROCESS_H
