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

#ifndef RA_FILESYSTEM_H
#define RA_FILESYSTEM_H

#include <stdint.h>
#include <string>
#include <vector>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"
#include "rapidassist/macros.h"

namespace ra { namespace filesystem {

  /// <summary>
  /// Normalizes a path.
  /// </summary>
  /// <param name="path">An valid file or directory path.</param>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  void NormalizePath(std::string & path);

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  uint32_t GetFileSize(const char * path);

  /// <summary>
  /// Returns the size of the given FILE* in bytes.
  /// </summary>
  /// <param name="f">An valid FILE pointer.</param>
  /// <returns>Returns the size of the given FILE* in bytes.</returns>
  uint32_t GetFileSize(FILE * f);

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  uint64_t GetFileSize64(const char * path);

  /// <summary>
  /// Returns the filename of the given path.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the filename of the given path.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string GetFilename(const char * path);

  /// <summary>
  /// Get the file name of a path without the extension.
  /// </summary>
  /// <param name="path">The file path of a file.</param>
  /// <returns>Returns the file name without the extension.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string GetFilenameWithoutExtension(const char * path);

  /// <summary>
  /// Determine if a file exists.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file exists. Returns false otherwise.</returns>
  bool FileExists(const char * path);

  /// <summary>
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  bool HasFileReadAccess(const char * path);

  /// <summary>
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  bool HasFileWriteAccess(const char * path);

  /// <summary>
  /// Determine if the current process has read access to a given directory.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow read access. Returns false otherwise.</returns>
  bool HasDirectoryReadAccess(const char * path);

  /// <summary>
  /// Determine if the current process has write access to a given directory.
  /// Note: the only way to detect if write access is available is to actually write a file.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow write access. Returns false otherwise.</returns>
  bool HasDirectoryWriteAccess(const char * path);

  /// <summary>
  /// DEPRECATED. Use HasFileReadAccess() instead.
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  DEPRECATED inline bool HasReadAccess(const char * path) { return HasFileReadAccess(path); }

  /// <summary>
  /// DEPRECATED. Use HasFileWriteAccess() instead.
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  DEPRECATED inline bool HasWriteAccess(const char * path) { return HasFileWriteAccess(path); }

  /// <summary>
  /// Find files in a directory / subdirectory.
  /// </summary>
  /// <param name="files">The list of files found.</param>
  /// <param name="path">An valid directory path.</param>
  /// <param name="depth">The search depth. Use 0 for finding files in directory path (without subdirectories). Use -1 for find all files in directory path (including subdirectories).</param>
  /// <returns>Returns true when files contains the list of files from directory path. Returns false otherwise.</returns>
  bool FindFiles(ra::strings::StringVector & files, const char * path, int depth);
  inline bool FindFiles(ra::strings::StringVector & files, const char * path) { return FindFiles(files, path, -1); }

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <param name="locations">The path locations where the file was found.</param>
  /// <returns>Returns true if the filename was found at least once. Returns false otherwise.</returns>
  bool FindFileFromPaths(const std::string & filename, ra::strings::StringVector & locations);

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <returns>Returns the first location where the file was found. Returns empty string otherwise.</returns>
  std::string FindFileFromPaths(const std::string & filename);

  /// <summary>
  /// Determine if a directory exists.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory exists. Returns false otherwise.</returns>
  bool DirectoryExists(const char * path);

  /// <summary>
  /// Creates the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was created (or already exists). Returns false otherwise.</returns>
  bool CreateDirectory(const char * path);

  /// <summary>
  /// Deletes the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was deleted (or does not exist). Returns false otherwise.</returns>
  bool DeleteDirectory(const char * path);

  /// <summary>
  /// Deletes the specified file.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file was deleted (or does not exist). Returns false otherwise.</returns>
  bool DeleteFile(const char * path);

  /// <summary>
  /// Returns the file name of a tempporary file.
  /// </summary>
  /// <returns>Returns the file name of a tempporary file.</returns>
  /// <remarks>This function returns an ASCII encoded string.</remarks>
  std::string GetTemporaryFileName();

  /// <summary>
  /// Returns the path of a temporary file.
  /// </summary>
  /// <returns>Returns the path of a tempporary file.</returns>
  std::string GetTemporaryFilePath();

  /// <summary>
  /// Returns the path of the temporary directory.
  /// </summary>
  /// <returns>Returns the path of the temporary directory.</returns>
  std::string GetTemporaryDirectory();

  /// <summary>
  /// Returns the parent element of a path. For files, it returns the file's directory. For directories, it returns the parent path.
  /// </summary>
  /// <param name="path">The input path to get the parent.</param>
  /// <returns>Returns the parent element of the given path.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string GetParentPath(const std::string & path);

  /// <summary>
  /// Convert a long file path to the short path form (8.3 format).
  /// If the system does not support automatic conversion, an estimated
  /// version is returned.
  /// </summary>
  /// <param name="path">The input path to convert.</param>
  /// <returns>Returns the short path form of the given path.</returns>
  std::string GetShortPathForm(const std::string & path);

  /// <summary>
  /// Splits a path into a directory and a filename.
  /// </summary>
  /// <param name="path">The input path to split.</param>
  /// <param name="directory">The output directory of the given path.</param>
  /// <param name="filename">The output file of the given path.</param>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  void SplitPath(const std::string & path, std::string & directory, std::string & filename);

  /// <summary>
  /// Splits a path into each element.
  /// </summary>
  /// <param name="path">The input path to split.</param>
  /// <param name="elements">The output list which contains all path elements.</param>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  void SplitPath(const std::string & path, std::vector<std::string> & elements);

  /// <summary>
  /// Convert an absolute path to a relative path based on the given absolute base path.
  /// </summary>
  /// <param name="base_path">The base path from which the relate path is constructed.</param>
  /// <param name="test_path">The full path that must be converted.</param>
  /// <returns>Returns a relative path from 'base_path' to 'test_path'. Returns an empty string on failure.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string MakeRelativePath(const std::string & base_path, const std::string & test_path);

  /// <summary>
  /// Gets the character that represents the path separator.
  /// </summary>
  /// <returns>Returns the character that represents the path separator.</returns>
  char GetPathSeparator();
  const char * GetPathSeparatorStr();

  /// <summary>
  /// Returns the current directory.
  /// </summary>
  /// <returns>Returns the current directory</returns>
  std::string GetCurrentDirectory();

  /// <summary>
  /// Returns the extension of a file.
  /// </summary>
  /// <param name="path">The valid path to a file.</param>
  /// <returns>Returns the extension of a file.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string GetFileExtention(const std::string & path);

  enum FileSizeEnum { BYTES, KILOBYTES, MEGABYTES, GIGABYTES, TERABYTES };

  /// <summary>
  /// Returns a given size in a user friendly format and units.
  /// </summary>
  /// <param name="size_in_bytes">The size in bytes</param>
  /// <returns>Returns a given size in a user friendly format and units.</returns>
  std::string GetUserFriendlySize(uint64_t size_in_bytes);

  /// <summary>
  /// Returns a given size in a user friendly format and units.
  /// </summary>
  /// <param name="size_in_bytes">The size in bytes</param>
  /// <param name="size_unit">The unit output format.</param>
  /// <returns>Returns a given size in a user friendly format and units.</returns>
  std::string GetUserFriendlySize(uint64_t size_in_bytes, FileSizeEnum size_unit);

  /// <summary>
  /// Returns the modified date of the given file.
  /// Note that the function returns the number of seconds elapsed since epoch since Jan 1st 1970.
  /// </summary>
  /// <param name="path">The valid path to a file.</param>
  /// <returns>Returns the modified date of the given file.</returns>
  uint64_t GetFileModifiedDate(const std::string & path);

  /// <summary>
  /// Determine if the given directory is empty.
  /// If the given directory contains a least one directory or a file, this function will return false.
  /// The directory entries `.` and `..` are ignored.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true if the given directory path is empty. Returns false otherwise.</returns>
  bool IsDirectoryEmpty(const std::string & path);

  /// <summary>
  /// Determine if the given path is an absolute path or not.
  /// </summary>
  /// <param name="path">An valid file or directory path.</param>
  /// <returns>Returns true if the given path is absolute. Returns false otherwise.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  bool IsAbsolutePath(const std::string & path);

  /// <summary>
  /// Determine if the given path is a root directory.
  /// </summary>
  /// <remarks>
  /// On Linux,   the root directory is "/".
  /// On Windows, there is one root directory per drive identified by a drive letter followed by ":\". For instance "C:\".
  /// This function is compatible with UTF-8 encoded strings.
  /// </remarks>
  /// <param name="path">An valid file or directory path.</param>
  /// <returns>Returns true if the given path is a root directory. Returns false otherwise.</returns>
  bool IsRootDirectory(const std::string & path);

  /// <summary>
  /// Resolves path that contains `..` or `.` elements to an absolute path.
  /// </summary>
  /// <remarks>
  /// On Linux,   one cannot walk down past the root: "/.."   is the same as "/".
  /// On Windows, one cannot walk down past the root: "C:\.." is the same as "C:\".
  /// </remarks>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the resolved path.</returns>
  /// <remarks>This function is compatible with UTF-8 encoded strings.</remarks>
  std::string ResolvePath(const std::string & path);

  /// <summary>
  /// Build an absolute path from the given relative path based on the directory of the current executable.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  std::string GetPathBasedOnCurrentProcess(const std::string & path);

  /// <summary>
  /// Build an absolute path from the given relative path based on the current directory.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  std::string GetPathBasedOnCurrentDirectory(const std::string & path);

  /// <summary>
  /// CopyFile() callback interface
  /// </summary>
  class IProgressReport {
  public:
    /// <summary>
    /// CopyFile() callback function.
    /// </summary>
    /// <param name="progress">The progress of the file copy. Ranges [0, 1] inclusive.</param>
    virtual void OnProgressReport(double progress) = 0;
  };

  /// <summary>
  /// CopyFile() callback function.
  /// </summary>
  /// <param name="progress">The progress of the file copy. Ranges [0, 1] inclusive.</param>
  typedef void(*ProgressReportCallback)(double);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFile(const std::string & source_path, const std::string & destination_path);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_functor">A valid IProgressReport pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFile(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_function">A valid ProgressReportCallback function pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFile(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function);

  /// <summary>
  /// Reads the first 'size' bytes of file 'path' and copy the binary data to 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The number of bytes to peek into the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool PeekFile(const std::string & path, size_t size, std::string & data);

  /// <summary>
  /// Reads the binary data of the given file into the 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadFile(const std::string & path, std::string & data);

  /// <summary>
  /// Writes the given binary data to a file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The data to write to the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteFile(const std::string & path, const std::string & data);

  /// <summary>
  /// Process a search and replace operation on the data of the given file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="old_value">The old value to replace by the new value.</param>
  /// <param name="new_value">The new value that replaces the old_value.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool FileReplace(const std::string & path, const std::string & old_value, const std::string & new_value);

  /// <summary>
  /// Reads a text file line by line and store the output into the 'lines' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The content of each line of text.</param>
  /// <param name="trim_newline_characters">Defines if the function should trim the new-line characters at the end of each lines.  See also 'insert_newline_characters' argument of function 'WriteTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadTextFile(const std::string & path, ra::strings::StringVector & lines, bool trim_newline_characters = true);

  /// <summary>
  /// Reads a text file and store the content into the 'content' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadTextFile(const std::string & path, std::string & content);

  /// <summary>
  /// Write the given content into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file to write into the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteTextFile(const std::string & path, const std::string & content);

  /// <summary>
  /// Writes the given lines of text into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The lines that we want to write to the file.</param>
  /// <param name="insert_newline_characters">Defines if the function should insert new-line characters at the end of each line. See also 'trim_newline_characters' argument of function 'ReadTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteTextFile(const std::string & path, const ra::strings::StringVector & lines, bool insert_newline_characters = true);

} //namespace filesystem
} //namespace ra

#endif //RA_FILESYSTEM_H
