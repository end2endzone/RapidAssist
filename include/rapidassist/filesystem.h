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

#include "strings.h"

namespace ra
{
  namespace filesystem
  {

    /// <summary>
    /// Normalizes a path.
    /// </summary>
    /// <param name="path">An valid file/folder path.</param>
    void normalizePath(std::string & path);

    /// <summary>
    /// Returns the size of the given file path in bytes.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns the size of the given file path in bytes.</returns>
    uint32_t getFileSize(const char * iPath);

    /// <summary>
    /// Returns the size of the given FILE* in bytes.
    /// </summary>
    /// <param name="f">An valid FILE pointer.</param>
    /// <returns>Returns the size of the given FILE* in bytes.</returns>
    uint32_t getFileSize(FILE * f);

    /// <summary>
    /// Returns the filename of the given path.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns the filename of the given path.</returns>
    std::string getFilename(const char * iPath);

    /// <summary>
    /// Determine if a file exists.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns true when the file exists. Returns false otherwise.</returns>
    bool fileExists(const char * iPath);

    /// <summary>
    /// Determine if the current process has read access to a given file.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns true when the file can be read. Returns false otherwise.</returns>
    bool hasReadAccess(const char * iPath);

    /// <summary>
    /// Determine if the current process has read access to a given file.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns true when the file can be write. Returns false otherwise.</returns>
    bool hasWriteAccess(const char * iPath);

    /// <summary>
    /// Find files in a directory / subdirectory.
    /// </summary>
    /// <param name="oFiles">The list of files found.</param>
    /// <param name="iPath">An valid folder path.</param>
    /// <param name="iDepth">The search depth. Use 0 for finding files in folder iPath (without subfolders). Use -1 for find all files in folder iPath (including subfolders). </param>
    /// <returns>Returns true when oFiles contains the list of files from folder iPath. Returns false otherwise.</returns>
    bool findFiles(ra::strings::StringVector & oFiles, const char * iPath, int iDepth);
    inline bool findFiles(ra::strings::StringVector & oFiles, const char * iPath) { return findFiles(oFiles, iPath, -1); }

    /// <summary>
    /// Determine if a folder exists.
    /// </summary>
    /// <param name="iPath">An valid folder path.</param>
    /// <returns>Returns true when the folder exists. Returns false otherwise.</returns>
    bool folderExists(const char * iPath);

    /// <summary>
    /// Creates the specified directory.
    /// </summary>
    /// <param name="iPath">An valid folder path.</param>
    /// <returns>Returns true when the folder was created (or already exists). Returns false otherwise.</returns>
    bool createFolder(const char * iPath);

    /// <summary>
    /// Deletes the specified directory.
    /// </summary>
    /// <param name="iPath">An valid folder path.</param>
    /// <returns>Returns true when the folder was deleted (or does not exist). Returns false otherwise.</returns>
    bool deleteFolder(const char * iPath);

    /// <summary>
    /// Deletes the specified file.
    /// </summary>
    /// <param name="iPath">An valid file path.</param>
    /// <returns>Returns true when the file was deleted (or does not exist). Returns false otherwise.</returns>
    bool deleteFile(const char * iPath);

    /// <summary>
    /// Returns the file name of a tempporary file.
    /// </summary>
    /// <returns>Returns the file name of a tempporary file.</returns>
    std::string getTemporaryFileName();

    /// <summary>
    /// Returns the path of a tempporary file.
    /// </summary>
    /// <returns>Returns the path of a tempporary file.</returns>
    std::string getTemporaryFilePath();

    /// <summary>
    /// Returns the parent element of a path. For files, returns the file's directory. For folders, returns the parent path
    /// </summary>
    /// <param name="iPath">The input path to split.</param>
    /// <returns>Returns the parent element of a path.</returns>
    std::string getParentPath(const std::string & iPath);

    /// <summary>
    /// Convert a long file path to the short path form (8.3 format).
    /// If the system does not support automatic conversion, an estimated
    /// version is returned.
    /// </summary>
    /// <param name="iPath">The input path to convert.</param>
    /// <returns>Returns the short path form of the given path.</returns>
    std::string getShortPathForm(const std::string & iPath);

    /// <summary>
    /// Splits a path into a folder and a filename.
    /// </summary>
    /// <param name="iPath">The input path to split.</param>
    /// <param name="oFolder">The output folder of the given path.</param>
    /// <param name="oFilename">The output file of the given path.</param>
    void splitPath(const std::string & iPath, std::string & oFolder, std::string & oFilename);

    /// <summary>
    /// Splits a path into each element.
    /// </summary>
    /// <param name="iPath">The input path to split.</param>
    /// <param name="oElements">The output list which contains all path elements.</param>
    void splitPath(const std::string & iPath, std::vector<std::string> & oElements);

    /// <summary>
    /// Gets the character that represents the path separator.
    /// </summary>
    /// <returns>Returns the character that represents the path separator.</returns>
    char getPathSeparator();
    const char * getPathSeparatorStr();

    /// <summary>
    /// Returns the current folder
    /// </summary>
    /// <returns>Returns the current folder</returns>
    std::string getCurrentFolder();

    /// <summary>
    /// Returns the extension of a file.
    /// </summary>
    /// <param name="iPath">The valid path to a file.</param>
    /// <returns>Returns the extension of a file.</returns>
    std::string getFileExtention(const std::string & iPath);

    enum FileSizeEnum {Bytes, Kilobytes, Megabytes, Gigabytes, Terabytes};

    /// <summary>
    /// Returns a given size in a user friendly format and units.
    /// </summary>
    /// <param name="iBytesSize">The size in bytes</param>
    /// <returns>Returns a given size in a user friendly format and units.</returns>
    std::string getUserFriendlySize(uint64_t iBytesSize);

    /// <summary>
    /// Returns a given size in a user friendly format and units.
    /// </summary>
    /// <param name="iBytesSize">The size in bytes</param>
    /// <param name="iUnit">The unit output format.</param>
    /// <returns>Returns a given size in a user friendly format and units.</returns>
    std::string getUserFriendlySize(uint64_t iBytesSize, FileSizeEnum iUnit);

    /// <summary>
    /// Returns the modified date of the given file.
    /// Note that the function returns the number of seconds elapsed since epoch since Jan 1st 1970.
    /// </summary>
    /// <param name="iPath">The valid path to a file.</param>
    /// <returns>Returns the modified date of the given file.</returns>
    uint64_t getFileModifiedDate(const std::string & iPath);

  } //namespace filesystem
} //namespace ra

#endif //RA_FILESYSTEM_H