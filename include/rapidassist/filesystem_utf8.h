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

#ifndef RA_FILESYSTEM_UTF8_H
#define RA_FILESYSTEM_UTF8_H

#include <stdint.h>
#include <string>
#include <vector>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/macros.h"

namespace ra { namespace filesystem {

#ifdef _WIN32 // UTF-8

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  uint32_t GetFileSizeUtf8(const char * path);

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  uint64_t GetFileSize64Utf8(const char * path);

  /// <summary>
  /// Determine if a file exists.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file exists. Returns false otherwise.</returns>
  bool FileExistsUtf8(const char * path);

  /// <summary>
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  bool HasFileReadAccessUtf8(const char * path);

  /// <summary>
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  bool HasFileWriteAccessUtf8(const char * path);

  /// <summary>
  /// Determine if the current process has read access to a given directory.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow read access. Returns false otherwise.</returns>
  bool HasDirectoryReadAccessUtf8(const char * path);

  /// <summary>
  /// Determine if the current process has write access to a given directory.
  /// Note: the only way to detect if write access is available is to actually write a file.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow write access. Returns false otherwise.</returns>
  bool HasDirectoryWriteAccessUtf8(const char * path);

  /// <summary>
  /// DEPRECATED. Use HasFileReadAccessUtf8() instead.
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  DEPRECATED inline bool HasReadAccessUtf8(const char * path) { return HasFileReadAccessUtf8(path); }

  /// <summary>
  /// DEPRECATED. Use HasFileWriteAccessUtf8() instead.
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  DEPRECATED inline bool HasWriteAccessUtf8(const char * path) { return HasFileWriteAccessUtf8(path); }

  /// <summary>
  /// Find files in a directory / subdirectory.
  /// </summary>
  /// <param name="files">The list of files found.</param>
  /// <param name="path">An valid directory path.</param>
  /// <param name="depth">The search depth. Use 0 for finding files in directory path (without subdirectories). Use -1 for find all files in directory path (including subdirectories).</param>
  /// <returns>Returns true when files contains the list of files from directory path. Returns false otherwise.</returns>
  bool FindFilesUtf8(ra::strings::StringVector & files, const char * path, int depth);

  /// <summary>
  /// Find files in a directory / subdirectory.
  /// </summary>
  /// <param name="files">The list of files found.</param>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when files contains the list of files from directory path. Returns false otherwise.</returns>
  inline bool FindFilesUtf8(ra::strings::StringVector & files, const char * path) { return FindFilesUtf8(files, path, -1); }

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <param name="locations">The path locations where the file was found.</param>
  /// <returns>Returns true if the filename was found at least once. Returns false otherwise.</returns>
  bool FindFileFromPathsUtf8(const std::string & filename, ra::strings::StringVector & locations);

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <returns>Returns the first location where the file was found. Returns empty string otherwise.</returns>
  std::string FindFileFromPathsUtf8(const std::string & filename);

  /// <summary>
  /// Determine if a directory exists.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory exists. Returns false otherwise.</returns>
  bool DirectoryExistsUtf8(const char * path);

  /// <summary>
  /// Creates the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was created (or already exists). Returns false otherwise.</returns>
  bool CreateDirectoryUtf8(const char * path);

  /// <summary>
  /// Deletes the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was deleted (or does not exist). Returns false otherwise.</returns>
  bool DeleteDirectoryUtf8(const char * path);

  /// <summary>
  /// Deletes the specified file.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file was deleted (or does not exist). Returns false otherwise.</returns>
  bool DeleteFileUtf8(const char * path);

  /// <summary>
  /// Returns the path of a tempporary file.
  /// </summary>
  /// <returns>Returns the path of a tempporary file.</returns>
  std::string GetTemporaryFilePathUtf8();

  /// <summary>
  /// Returns the path of the temporary directory.
  /// </summary>
  /// <returns>Returns the path of the temporary directory.</returns>
  std::string GetTemporaryDirectoryUtf8();

  /// <summary>
  /// Returns the current directory.
  /// </summary>
  /// <returns>Returns the current directory</returns>
  std::string GetCurrentDirectoryUtf8();

  /// <summary>
  /// Returns the modified date of the given file.
  /// Note that the function returns the number of seconds elapsed since epoch since Jan 1st 1970.
  /// </summary>
  /// <param name="path">The valid path to a file.</param>
  /// <returns>Returns the modified date of the given file.</returns>
  uint64_t GetFileModifiedDateUtf8(const std::string & path);

  /// <summary>
  /// Determine if the given directory is empty.
  /// If the given directory contains a least one directory or a file, this function will return false.
  /// The directory entries `.` and `..` are ignored.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true if the given directory path is empty. Returns false otherwise.</returns>
  bool IsDirectoryEmptyUtf8(const std::string & path);

  /// <summary>
  /// Build an absolute path from the given relative path based on the directory of the current executable.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  std::string GetPathBasedOnCurrentProcessUtf8(const std::string & path);

  /// <summary>
  /// Build an absolute path from the given relative path based on the current directory.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  std::string GetPathBasedOnCurrentDirectoryUtf8(const std::string & path);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_functor">A valid IProgressReport pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, ra::filesystem::IProgressReport * progress_functor);

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_function">A valid ProgressReportCallback function pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, ra::filesystem::ProgressReportCallback progress_function);

  /// <summary>
  /// Reads the first 'size' bytes of file 'path' and copy the binary data to 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The number of bytes to peek into the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool PeekFileUtf8(const std::string & path, size_t size, std::string & data);

  /// <summary>
  /// Reads the binary data of the given file into the 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadFileUtf8(const std::string & path, std::string & data);

  /// <summary>
  /// Writes the given binary data to a file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The data to write to the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteFileUtf8(const std::string & path, const std::string & data);

  /// <summary>
  /// Process a search and replace operation on the data of the given file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="old_value">The old value to replace by the new value.</param>
  /// <param name="new_value">The new value that replaces the old_value.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool FileReplaceUtf8(const std::string & path, const std::string & old_value, const std::string & new_value);

  /// <summary>
  /// Reads a text file line by line and store the output into the 'lines' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The content of each line of text.</param>
  /// <param name="trim_newline_characters">Defines if the function should trim the new-line characters at the end of each lines.  See also 'insert_newline_characters' argument of function 'WriteTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadTextFileUtf8(const std::string & path, ra::strings::StringVector & lines, bool trim_newline_characters = true);

  /// <summary>
  /// Reads a text file and store the content into the 'content' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool ReadTextFileUtf8(const std::string & path, std::string & content);

  /// <summary>
  /// Write the given content into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file to write into the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteTextFileUtf8(const std::string & path, const std::string & content);

  /// <summary>
  /// Writes the given lines of text into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The lines that we want to write to the file.</param>
  /// <param name="insert_newline_characters">Defines if the function should insert new-line characters at the end of each line. See also 'trim_newline_characters' argument of function 'ReadTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  bool WriteTextFileUtf8(const std::string & path, const ra::strings::StringVector & lines, bool insert_newline_characters = true);

#elif defined(__linux__) || defined(__APPLE__) // UTF-8

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline uint32_t GetFileSizeUtf8(const char * path) { return GetFileSize(path); }

  /// <summary>
  /// Returns the size of the given file path in bytes.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns the size of the given file path in bytes.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline uint64_t GetFileSize64Utf8(const char * path) { return GetFileSize64(path); }

  /// <summary>
  /// Determine if a file exists.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file exists. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool FileExistsUtf8(const char * path) { return FileExists(path); }

  /// <summary>
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool HasFileReadAccessUtf8(const char * path) { return HasFileReadAccess(path); }

  /// <summary>
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool HasFileWriteAccessUtf8(const char * path) { return HasFileWriteAccess(path); }

  /// <summary>
  /// Determine if the current process has read access to a given directory.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow read access. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool HasDirectoryReadAccessUtf8(const char * path) { return HasDirectoryReadAccess(path); }

  /// <summary>
  /// Determine if the current process has write access to a given directory.
  /// Note: the only way to detect if write access is available is to actually write a file.
  /// </summary>
  /// <param name="path">A valid directory path.</param>
  /// <returns>Returns true when the directory allow write access. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool HasDirectoryWriteAccessUtf8(const char * path) { return HasDirectoryWriteAccess(path); }

  /// <summary>
  /// DEPRECATED. Use HasFileReadAccessUtf8() instead.
  /// Determine if the current process has read access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
  DEPRECATED inline bool HasReadAccessUtf8(const char * path) { return HasFileReadAccessUtf8(path); }

  /// <summary>
  /// DEPRECATED. Use HasFileWriteAccessUtf8() instead.
  /// Determine if the current process has write access to a given file.
  /// </summary>
  /// <param name="path">A valid file path.</param>
  /// <returns>Returns true when the file can be write to. Returns false otherwise.</returns>
  DEPRECATED inline bool HasWriteAccessUtf8(const char * path) { return HasFileWriteAccessUtf8(path); }

  /// <summary>
  /// Find files in a directory / subdirectory.
  /// </summary>
  /// <param name="files">The list of files found.</param>
  /// <param name="path">An valid directory path.</param>
  /// <param name="depth">The search depth. Use 0 for finding files in directory path (without subdirectories). Use -1 for find all files in directory path (including subdirectories).</param>
  /// <returns>Returns true when files contains the list of files from directory path. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool FindFilesUtf8(ra::strings::StringVector & files, const char * path, int depth) { return FindFiles(files, path, depth); }

  /// <summary>
  /// Find files in a directory / subdirectory.
  /// </summary>
  /// <param name="files">The list of files found.</param>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when files contains the list of files from directory path. Returns false otherwise.</returns>
  inline bool FindFilesUtf8(ra::strings::StringVector & files, const char * path) { return FindFilesUtf8(files, path, -1); }

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <param name="locations">The path locations where the file was found.</param>
  /// <returns>Returns true if the filename was found at least once. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool FindFileFromPathsUtf8(const std::string & filename, ra::strings::StringVector & locations) { return FindFileFromPaths(filename, locations); }

  /// <summary>
  /// Finds a file using the PATH environment variable.
  /// </summary>
  /// <param name="filename">The filename that we are searching for.</param>
  /// <returns>Returns the first location where the file was found. Returns empty string otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string FindFileFromPathsUtf8(const std::string & filename) { return FindFileFromPaths(filename); }

  /// <summary>
  /// Determine if a directory exists.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory exists. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool DirectoryExistsUtf8(const char * path) { return DirectoryExists(path); }

  /// <summary>
  /// Creates the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was created (or already exists). Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool CreateDirectoryUtf8(const char * path) { return CreateDirectory(path); }

  /// <summary>
  /// Deletes the specified directory.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true when the directory was deleted (or does not exist). Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool DeleteDirectoryUtf8(const char * path) { return DeleteDirectory(path); }

  /// <summary>
  /// Deletes the specified file.
  /// </summary>
  /// <param name="path">An valid file path.</param>
  /// <returns>Returns true when the file was deleted (or does not exist). Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool DeleteFileUtf8(const char * path) { return DeleteFile(path); }

  /// <summary>
  /// Returns the path of a tempporary file.
  /// </summary>
  /// <returns>Returns the path of a tempporary file.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetTemporaryFilePathUtf8() { return GetTemporaryFilePath(); }

  /// <summary>
  /// Returns the path of the temporary directory.
  /// </summary>
  /// <returns>Returns the path of the temporary directory.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetTemporaryDirectoryUtf8() { return GetTemporaryDirectory(); }

  /// <summary>
  /// Returns the current directory.
  /// </summary>
  /// <returns>Returns the current directory</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetCurrentDirectoryUtf8() { return GetCurrentDirectory(); }

  /// <summary>
  /// Returns the modified date of the given file.
  /// Note that the function returns the number of seconds elapsed since epoch since Jan 1st 1970.
  /// </summary>
  /// <param name="path">The valid path to a file.</param>
  /// <returns>Returns the modified date of the given file.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline uint64_t GetFileModifiedDateUtf8(const std::string & path) { return GetFileModifiedDate(path); }

  /// <summary>
  /// Determine if the given directory is empty.
  /// If the given directory contains a least one directory or a file, this function will return false.
  /// The directory entries `.` and `..` are ignored.
  /// </summary>
  /// <param name="path">An valid directory path.</param>
  /// <returns>Returns true if the given directory path is empty. Returns false otherwise.</returns>
  inline bool IsDirectoryEmptyUtf8(const std::string & path) { return IsDirectoryEmpty(path); }

  /// <summary>
  /// Build an absolute path from the given relative path based on the directory of the current executable.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetPathBasedOnCurrentProcessUtf8(const std::string & path) { return GetPathBasedOnCurrentProcess(path); }

  /// <summary>
  /// Build an absolute path from the given relative path based on the current directory.
  /// </summary>
  /// <param name="path">An valid relative file or directory path.</param>
  /// <returns>Returns the absolute path.</returns>
  /// <remarks>If the given path is already an absolute path, the given path is returned.</remarks>
  /// <remarks>
  /// If the given path is already an absolute path, the given path is returned.
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline std::string GetPathBasedOnCurrentDirectoryUtf8(const std::string & path) { return GetPathBasedOnCurrentDirectory(path); }

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path) { return CopyFile(source_path, destination_path); }

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_functor">A valid IProgressReport pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor) { return CopyFile(source_path, destination_path, progress_functor); }

  /// <summary>
  /// Copy a file to another destination.
  /// </summary>
  /// <param name="source_path">The source file path to copy.</param>
  /// <param name="destination_path">The destination file path.</param>
  /// <param name="progress_function">A valid ProgressReportCallback function pointer to handle the copy callback.</param>
  /// <returns>Returns true if file copy is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function) { return CopyFile(source_path, destination_path, progress_function); }

  /// <summary>
  /// Reads the first 'size' bytes of file 'path' and copy the binary data to 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The number of bytes to peek into the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool PeekFileUtf8(const std::string & path, size_t size, std::string & data) { return PeekFile(path, size, data); }

  /// <summary>
  /// Reads the binary data of the given file into the 'data' variable.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The variable that will contains the readed bytes.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool ReadFileUtf8(const std::string & path, std::string & data) { return ReadFile(path, data); }

  /// <summary>
  /// Writes the given binary data to a file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="data">The data to write to the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool WriteFileUtf8(const std::string & path, const std::string & data) { return WriteFile(path, data); }

  /// <summary>
  /// Process a search and replace operation on the data of the given file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="old_value">The old value to replace by the new value.</param>
  /// <param name="new_value">The new value that replaces the old_value.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool FileReplaceUtf8(const std::string & path, const std::string & old_value, const std::string & new_value) { return FileReplace(path, old_value, new_value); }

  /// <summary>
  /// Reads a text file line by line and store the output into the 'lines' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The content of each line of text.</param>
  /// <param name="trim_newline_characters">Defines if the function should trim the new-line characters at the end of each lines.  See also 'insert_newline_characters' argument of function 'WriteTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool ReadTextFileUtf8(const std::string & path, ra::strings::StringVector & lines, bool trim_newline_characters = true) { return ReadTextFile(path, lines, trim_newline_characters); }

  /// <summary>
  /// Reads a text file and store the content into the 'content' variable.
  /// Note that on Windows platform, CRLF line ending will be converted to CR line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool ReadTextFileUtf8(const std::string & path, std::string & content) { return ReadTextFile(path, content); }

  /// <summary>
  /// Write the given content into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="content">The content of the text file to write into the file.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool WriteTextFileUtf8(const std::string & path, const std::string & content) { return WriteTextFile(path, content); }

  /// <summary>
  /// Writes the given lines of text into a text file.
  /// Note that on Windows platform, CR line ending will be converted to CRLF line ending.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The lines that we want to write to the file.</param>
  /// <param name="insert_newline_characters">Defines if the function should insert new-line characters at the end of each line. See also 'trim_newline_characters' argument of function 'ReadTextFile()'.</param>
  /// <returns>Returns true when the function is successful. Returns false otherwise.</returns>
  /// <remarks>
  /// On Linux, this function delegates to the non-utf8 function (the function with the same name without the 'Utf8' postfix).
  /// It provides cross-platform compatibility for Windows users.
  /// </remarks>
  inline bool WriteTextFileUtf8(const std::string & path, const ra::strings::StringVector & lines, bool insert_newline_characters = true) { return WriteTextFile(path, lines, insert_newline_characters); }

#endif // UTF-8

} //namespace filesystem
} //namespace ra

#endif //RA_FILESYSTEM_UTF8_H
