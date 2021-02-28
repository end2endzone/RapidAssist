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

#ifndef RA_TESTING_UTF8_H
#define RA_TESTING_UTF8_H

#include <stdint.h>

#include "rapidassist/config.h"
#include "rapidassist/testing.h"

namespace ra { namespace testing {

#ifdef _WIN32 // UTF-8

  /// <summary>
  /// Returns true if the content of two files is identical.
  /// </summary>
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="reason">A textual reason why the files are not identical. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of textual differences. Use -1 for finding all differences. Use 1 to return as soon as a difference is found.</param>
  /// <returns>Returns true if the content of two files is identical. Returns false otherwise.</returns>
  bool IsFileEqualsUtf8(const char* file1, const char* file2, std::string & reason, size_t max_differences);

  inline bool IsFileEqualsUtf8(const char* file1, const char* file2) { std::string reason; return IsFileEqualsUtf8(file1, file2, reason, 1 /*return ASAP*/); }
  inline bool IsFileEqualsUtf8(const char* file1, const char* file2, std::string & reason) { return IsFileEqualsUtf8(file1, file2, reason, 1 /*return ASAP*/); }

  /// <summary>
  /// Returns the location of all differences in two files.
  /// </summary>
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="differences">The output list of all differences within both files. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of differences.</param>
  /// <returns>Returns the location of all differences in two files. Returns false otherwise.</returns>
  bool GetFileDifferencesUtf8(const char* file1, const char* file2, std::vector<FileDiff> & differences, size_t max_differences);

  /// <summary>
  /// Returns true if the given text is found in a file.
  /// </summary>
  /// <param name="path">The path of the search file.</param>
  /// <param name="value">The search value.</param>
  /// <param name="line_index">The line number where value is found.</param>
  /// <param name="character_index">The character offset within the line.</param>
  /// <returns>Returns true if the given text is found in a file. Returns false otherwise.</returns>
  bool FindInFileUtf8(const char* path, const char* value, int & line_index, int & character_index);

  /// <summary>
  /// Creates a file of the given size. All bytes are sequential.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size in bytes of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFileUtf8(const char * path, size_t size);

  /// <summary>
  /// Creates text file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFileUtf8(const char * path);

  /// <summary>
  /// Creates a large file.
  /// </summary>
  /// <remarks>
  /// https://en.wikipedia.org/wiki/Sparse_file
  /// https://stackoverflow.com/questions/43126760/what-is-a-sparse-file-and-why-do-we-need-it
  /// </remarks>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size of the file, in bytes.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFileSparseUtf8(const char * path, uint64_t size);

  /// <summary>
  /// Modify a given byte with the specified value.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="offset">The offset of the modified byte.</param>
  /// <param name="value">The value of the replacement.</param>
  void ChangeFileContentUtf8(const char * path, size_t offset, unsigned char value);

  /// <summary>
  /// Copy the file of the current process to another file.
  /// </summary>
  /// <param name="target_path">Target file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  bool CloneExecutableFileUtf8(const std::string & target_path, std::string & error_message);

  /// <summary>
  /// Copy the file of the current process to a temporary file.
  /// </summary>
  /// <param name="output_path">Output file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  bool CloneExecutableTempFileUtf8(std::string & output_path, std::string & error_message);

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Returns true if the content of two files is identical.
  /// </summary>
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="reason">A textual reason why the files are not identical. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of textual differences. Use -1 for finding all differences. Use 1 to return as soon as a difference is found.</param>
  /// <returns>Returns true if the content of two files is identical. Returns false otherwise.</returns>
  inline bool IsFileEqualsUtf8(const char* file1, const char* file2) { return IsFileEquals(file1, file2); }
  inline bool IsFileEqualsUtf8(const char* file1, const char* file2, std::string & reason) { return IsFileEquals(file1, file2, reason); }
  inline bool IsFileEqualsUtf8(const char* file1, const char* file2, std::string & reason, size_t max_differences) { return IsFileEquals(file1, file2, reason, max_differences); }

  /// <summary>
  /// Returns the location of all differences in two files.
  /// </summary>
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="differences">The list of all differences within both files. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of differences.</param>
  /// <returns>Returns the location of all differences in two files. Returns false otherwise.</returns>
  inline bool GetFileDifferencesUtf8(const char* file1, const char* file2, std::vector<FileDiff> & differences, size_t max_differences) { return GetFileDifferences(file1, file2, differences, max_differences); }

  /// <summary>
  /// Returns true if the given text is found in a file.
  /// </summary>
  /// <param name="path">The path of the search file.</param>
  /// <param name="value">The search value.</param>
  /// <param name="line_index">The line number where value is found.</param>
  /// <param name="character_index">The character offset within the line.</param>
  /// <returns>Returns true if the given text is found in a file. Returns false otherwise.</returns>
  inline bool FindInFileUtf8(const char* path, const char* value, int & line_index, int & character_index) { return FindInFile(path, value, line_index, character_index); }

  /// <summary>
  /// Creates a file of the given size. All bytes are sequential.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size in bytes of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  inline bool CreateFileUtf8(const char * path, size_t size) { return CreateFile(path, size); }

  /// <summary>
  /// Creates text file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  inline bool CreateFileUtf8(const char * path) { return CreateFile(path); }

  /// <summary>
  /// Creates a large file.
  /// </summary>
  /// <remarks>
  /// https://en.wikipedia.org/wiki/Sparse_file
  /// https://stackoverflow.com/questions/43126760/what-is-a-sparse-file-and-why-do-we-need-it
  /// </remarks>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size of the file, in bytes.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  inline bool CreateFileSparseUtf8(const char * path, uint64_t size) { return CreateFileSparse(path, size); }

  /// <summary>
  /// Modify a given byte with the specified value.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="offset">The offset of the modified byte.</param>
  /// <param name="value">The value of the replacement.</param>
  inline void ChangeFileContentUtf8(const char * path, size_t offset, unsigned char value) { ChangeFileContent(path, offset, value); }

  /// <summary>
  /// Copy the file of the current process to another file.
  /// </summary>
  /// <param name="target_path">Target file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  inline bool CloneExecutableFileUtf8(const std::string & target_path, std::string & error_message) { return CloneExecutableFile(target_path, error_message); }

  /// <summary>
  /// Copy the file of the current process to a temporary file.
  /// </summary>
  /// <param name="output_path">Output file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  inline bool CloneExecutableTempFileUtf8(std::string & output_path, std::string & error_message) { return CloneExecutableTempFile(output_path, error_message); }

#endif // UTF-8

} //namespace testing
} //namespace ra

#endif //RA_TESTING_UTF8_H
