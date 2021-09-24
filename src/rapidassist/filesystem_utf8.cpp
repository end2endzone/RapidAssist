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

#include "rapidassist/environment.h"
#include "rapidassist/environment_utf8.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/random.h"
#include "rapidassist/process.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/unicode.h"

#include <algorithm>  //for std::transform(), sort()
#include <string.h>   //for strdup()
#include <stdlib.h>   //for realpath()

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stat
#define stat64 _stat64
#define __getcwd _getcwd
#define __chdir _chdir
#define __rmdir _rmdir
#include <direct.h> //for _chdir(), _getcwd()
#include <shlwapi.h> // for PathIsDirectoryEmptyA()
#pragma comment(lib, "Shlwapi.lib") //for PathIsDirectoryEmptyA()
#include <Windows.h> //for GetShortPathName()
#include "rapidassist/undef_windows_macros.h"
#elif defined(__linux__) || defined(__APPLE__)
#define __chdir chdir
#define __getcwd getcwd
#define __rmdir rmdir
#include <unistd.h> //for getcwd()
#include <dirent.h> //for opendir() and closedir()
#endif

#if defined(__linux__)
#include <linux/limits.h> //for PATH_MAX
#elif defined(__APPLE__)
#include <limits.h> //for PATH_MAX
#endif

namespace ra { namespace filesystem {

#ifdef _WIN32 // UTF-8

  struct greater {
    template<class T>
    bool operator()(T const &a, T const &b) const { return a > b; }
  };

  uint32_t GetFileSizeUtf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return 0;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct _stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      return sb.st_size;
    }

    return 0;
  }

  uint64_t GetFileSize64Utf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return 0;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct _stat64 sb;
    if (_wstat64(pathW.c_str(), &sb) == 0) {
      return sb.st_size;
    }

    return 0;
  }

  bool FileExistsUtf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct _stat64 sb;
    if (_wstat64(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IFREG) == S_IFREG)
        return true;
    }
    return false;
  }

  bool HasFileReadAccessUtf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct _stat64 sb;
    if (_wstat64(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IREAD) == S_IREAD)
        return true;
    }
    return false;
  }

  bool HasFileWriteAccessUtf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct _stat64 sb;
    if (_wstat64(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IWRITE) == S_IWRITE)
        return true;
    }
    return false;
  }

  bool HasDirectoryReadAccessUtf8(const char * path)
  {
    //Check if the directory already exists
    if (!ra::filesystem::DirectoryExistsUtf8(path))
        return false; //Directory not found. Denied read access.

    //Search for files in directory (not recursive)
    ra::strings::StringVector files;
    bool success = ra::filesystem::FindFilesUtf8(files, path, 0);
    if (!success)
      return false;

    return true;
  }

  bool HasDirectoryWriteAccessUtf8(const char * path)
  {
    //Check if the directory already exists
    if (!ra::filesystem::DirectoryExistsUtf8(path))
        return false; //Directory not found. Denied write access.

    //Generate a random filename to use as a "temporary file".
    std::string filename = ra::filesystem::GetTemporaryFileName();

    //Try to create a file. This will validate that we have write access to the directory.
    std::string file_path = std::string(path) + ra::filesystem::GetPathSeparatorStr() + filename;
    static const std::string data = __FUNCTION__;
    bool file_created = ra::filesystem::WriteFileUtf8(file_path, data);
    if (!file_created)
      return false; //Write is denied

    //Write is granted

    //Cleaning up
    bool deleted = ra::filesystem::DeleteFileUtf8(file_path.c_str());

    return true;
  }

  //declared in filesystem.cpp
  extern bool ProcessDirectoryEntry(ra::strings::StringVector & files, const char * directory_path, const std::string & filename, bool is_directory, int depth, bool use_utf8);

  bool FindFilesUtf8(ra::strings::StringVector & files, const char * path, int depth) {
    if (path == NULL)
      return false;

    //Build a *.* query
    std::string query = path;
    NormalizePath(query);
    query << "\\*";

    std::wstring queryW = ra::unicode::Utf8ToUnicode(query);

    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW(queryW.c_str(), &find_data);

    if (hFind == INVALID_HANDLE_VALUE)
      return false;

    //process directory entry
    std::wstring filenameW = find_data.cFileName;
    std::string filename_utf8 = ra::unicode::UnicodeToUtf8(filenameW); //convert from Wide character (Unicode) to UTF-8
    bool is_directory = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    bool is_junction = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0); //or JUNCTION, SYMLINK or MOUNT_POINT
    bool result = ProcessDirectoryEntry(files, path, filename_utf8, is_directory, depth, true);
    if (!result) {
      //Warning: Current user is not able to browse this directory.
      //For instance:
      //
      //  C:\Documents and Settings>dir
      //   Volume in drive C is WINDOWS
      //   Volume Serial Number is Z00Z-Z000
      // 
      //   Directory of C:\Documents and Settings
      // 
      //  File Not Found
    }

    //next files in directory
    while (FindNextFileW(hFind, &find_data)) {
      filenameW = find_data.cFileName;
      filename_utf8 = ra::unicode::UnicodeToUtf8(filenameW); //convert from Wide character (Unicode) to UTF-8
      bool is_directory = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
      bool is_junction = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0); //or JUNCTION, SYMLINK or MOUNT_POINT
      bool result = ProcessDirectoryEntry(files, path, filename_utf8, is_directory, depth, true);
      if (!result) {
        //Warning: Current user is not able to browse this directory.
      }
    }
    FindClose(hFind);
    return true;
  }

  bool FindFileFromPathsUtf8(const std::string & filename, ra::strings::StringVector & locations) {
    locations.clear();

    //define separator in PATH environment variable
    static const char * separator = ";";

    std::string path_env_utf8 = ra::environment::GetEnvironmentVariableUtf8("PATH");
    if (path_env_utf8.empty())
      return false;

    //split each path
    ra::strings::StringVector paths;
    ra::strings::Split(paths, path_env_utf8, separator);

    //search within all paths
    bool found = false;
    for (size_t i = 0; i < paths.size(); i++) {
      std::string path_utf8 = paths[i];

      //Expand the path in case it contains environment variables
      path_utf8 = ra::environment::ExpandUtf8(path_utf8.c_str());

      //Remove the last path separator (\ or / characters)
      ra::filesystem::NormalizePath(path_utf8);

      //append the query filename
      path_utf8 += ra::filesystem::GetPathSeparatorStr();
      path_utf8 += filename;

      //look if the file exists
      if (ra::filesystem::FileExistsUtf8(path_utf8.c_str())) {
        //found a possible match for filename
        locations.push_back(path_utf8);
        found = true;
      }
    }

    return found;
  }

  std::string FindFileFromPathsUtf8(const std::string & filename) {
    ra::strings::StringVector locations;
    bool found = FindFileFromPathsUtf8(filename, locations);
    if (!found || locations.size() == 0)
      return "";
    const std::string & first = locations[0];
    return first;
  }

  bool DirectoryExistsUtf8(const char * path) {
    if (path == NULL || path[0] == '\0')
      return false;

#ifdef _WIN32
    //Note: This was true before fixing #71.
    //Note that the current windows implementation of DirectoryExists() uses the _stat() API and the implementation has issues with junctions and symbolink link.
    //For instance, 'C:\Users\All Users\Favorites' exists but 'C:\Users\All Users' don't.
#endif

    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    struct stat64 sb;
    if (_wstat64(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IFDIR) == S_IFDIR)
        return true;
    }
    return false;
  }

  bool CreateDirectoryUtf8(const char * path) {
    if (path == NULL)
      return false;

    if (DirectoryExistsUtf8(path))
      return true;

    //directory does not already exists and must be created

    //inspired from https://stackoverflow.com/a/675193
    char *pp;
    char *sp;
    int   status;
    char separator = GetPathSeparator();
    char *copypath = _strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, separator)) != 0) {
      if (sp != pp) {
        /* Neither root nor double slash in path */
        *sp = '\0';
        
        std::wstring copypathW = ra::unicode::Utf8ToUnicode(copypath);

        status = _wmkdir(copypathW.c_str());
        if (status == -1 && strlen(copypath) == 2 && copypath[1] == ':') //issue #27
          status = 0; //fix for _mkdir("C:") like
        int errno_copy = errno;
        if (status == -1 && errno == EEXIST)
          status = 0; //File already exist

        if (status != 0) {
          //directory already exists?
          if (DirectoryExistsUtf8(copypath)) {
            status = 0;
          }
        }
        *sp = separator;
      }
      pp = sp + 1;
    }
    if (status == 0) {
      std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
      status = _wmkdir(pathW.c_str());
    }
    free(copypath);
    return (status == 0);
  }

  bool DeleteDirectoryUtf8(const char * path) {
    if (path == NULL)
      return false;

    if (!DirectoryExistsUtf8(path))
      return true;

    //directory exists and must be deleted

    //find all files and directories in specified directory
    ra::strings::StringVector files;
    bool found = FindFilesUtf8(files, path);
    if (!found)
      return false;

    //sort files in reverse order
    //this allows deleting sub-directories and sub-files first
    std::sort(files.begin(), files.end(), greater());

    //process files and directories
    for (size_t i = 0; i < files.size(); i++) {
      const std::string & direntry = files[i];
      if (FileExistsUtf8(direntry.c_str())) {
        bool deleted = DeleteFileUtf8(direntry.c_str());
        if (!deleted)
          return false; //failed to delete file
      }
      else {
        //assume direntry is a directory
        std::wstring direntryW = ra::unicode::Utf8ToUnicode(direntry);
        int result = _wrmdir(direntryW.c_str());
        if (result != 0)
          return false; //failed deleting directory.
      }
    }

    //delete the specified directory
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    int result = _wrmdir(pathW.c_str());
    return (result == 0);
  }

  bool DeleteFileUtf8(const char * path) {
    if (path == NULL)
      return false;

    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    int result = _wremove(pathW.c_str());
    return (result == 0);
  }

  std::string GetTemporaryFilePathUtf8() {
    std::string temp_dir = GetTemporaryDirectoryUtf8();
    std::string rnd_path = temp_dir + GetPathSeparator() + GetTemporaryFileName();
    return rnd_path;
  }

  std::string GetTemporaryDirectoryUtf8() {
#ifdef _WIN32
    std::string temp = environment::GetEnvironmentVariableUtf8("TEMP");
#elif __linux__
    std::string temp = "/tmp";
#elif __APPLE__
    //std::string temp = environment::GetEnvironmentVariable("TMPDIR");
    std::string temp = "/tmp";
#endif
    return temp;
  }

  std::string GetCurrentDirectoryUtf8() {
    std::wstring curdirW = _wgetcwd(NULL, 0);
    std::string curdir = ra::unicode::UnicodeToUtf8(curdirW);
    return curdir;
  }

  uint64_t GetFileModifiedDateUtf8(const std::string & path) {
    struct stat64 result;
    uint64_t mod_time = 0;
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    if (_wstat64(pathW.c_str(), &result) == 0) {
      mod_time = result.st_mtime;
    }
    return mod_time;
  }

  bool IsDirectoryEmptyUtf8(const std::string & path) {
#ifdef _WIN32
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    if (PathIsDirectoryEmptyW(pathW.c_str()) == TRUE)
      return true;
    return false;
#else
    return false;
#endif
  }

  std::string GetPathBasedOnCurrentProcessUtf8(const std::string & path) {
    if (IsAbsolutePath(path))
      return path;

    std::string dir = ra::process::GetCurrentProcessDirUtf8();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(path);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }

  std::string GetPathBasedOnCurrentDirectoryUtf8(const std::string & path) {
    if (IsAbsolutePath(path))
      return path;

    std::string dir = ra::filesystem::GetCurrentDirectoryUtf8();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(path);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }

  extern bool CopyFileInternal(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor, ProgressReportCallback progress_function, bool force_win32_utf8);

  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path) {
    return CopyFileInternal(source_path, destination_path, NULL, NULL, true);
  }

  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor) {
    return CopyFileInternal(source_path, destination_path, progress_functor, NULL, true);
  }

  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function) {
    return CopyFileInternal(source_path, destination_path, NULL, progress_function, true);
  }

  bool PeekFileUtf8(const std::string & path, size_t size, std::string & data) {
    //static const std::string EMPTY;
    data.clear();

    //validate if file exists
    if (!ra::filesystem::FileExistsUtf8(path.c_str()))
      return false;

    //allocate a buffer which can hold the data of the peek size
    uint64_t file_size = ra::filesystem::GetFileSize64Utf8(path.c_str());
    uint64_t max_read_size = (file_size < (uint64_t)size ? file_size : (uint64_t)size);

    //validates empty files 
    if (max_read_size == 0)
      return true;

    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE * f = _wfopen(pathW.c_str(), L"rb");
    if (!f)
      return false;

    //allocate a buffer to hold the content
    data.resize(max_read_size, 0);
    char * buffer = &data[0];
    char * last = &data[data.size() - 1];
    bool is_buffer_size_ok = (data.size() == max_read_size);
    bool is_contiguous = ((last - buffer + 1) == max_read_size);
    if (!is_buffer_size_ok || !is_contiguous) {
      fclose(f);
      return false;
    }

    //read the data
    size_t read_size = fread(buffer, 1, max_read_size, f);
    if (read_size != max_read_size) {
      fclose(f);
      return false;
    }

    fclose(f);

    bool success = (data.size() == max_read_size);
    return success;
  }

  bool ReadFileUtf8(const std::string & path, std::string & data) {
    //validate if file exists
    if (!ra::filesystem::FileExistsUtf8(path.c_str()))
      return false;

    uint32_t file_size = ra::filesystem::GetFileSizeUtf8(path.c_str());

    //validates empty files 
    if (file_size == 0)
      return true;

    bool readed = PeekFileUtf8(path, file_size, data);
    return readed;
  }

  bool WriteFileUtf8(const std::string & path, const std::string & data) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE * f = _wfopen(pathW.c_str(), L"wb");
    if (!f)
      return false;

    size_t size_write = fwrite(data.c_str(), 1, data.size(), f);

    fclose(f);

    bool success = (data.size() == size_write);
    return success;
  }

  bool FileReplaceUtf8(const std::string & path, const std::string & old_value, const std::string & new_value) {
    std::string data;
    if (!ReadFileUtf8(path, data))
      return false;

    int num_finding = ra::strings::Replace(data, old_value, new_value);

    //does the file was modified?
    if (num_finding) {
      //yes, write modifications to the file
      if (!WriteFileUtf8(path, data))
        return false;
    }

    return true;
  }

  bool ReadTextFileUtf8(const std::string & path, ra::strings::StringVector & lines, bool trim_newline_characters) {
    lines.clear();

    static const int BUFFER_SIZE = 10240;
    char buffer[BUFFER_SIZE];

    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE* f = _wfopen(pathW.c_str(), L"r");
    if (!f)
      return false;

    while (fgets(buffer, BUFFER_SIZE, f) != NULL) {
      if (trim_newline_characters) {
        //remove last CRLF at the end of the string
        ra::strings::RemoveEol(buffer);
      }

      std::string line = buffer;
      lines.push_back(line);
    }
    fclose(f);
    return true;
  }

  bool ReadTextFileUtf8(const std::string & path, std::string & content) {
    ra::strings::StringVector lines;
    bool success = ReadTextFileUtf8(path.c_str(), lines, false);
    if (!success)
      return false;

    //merge all lines (including newline characters)
    content = ra::strings::Join(lines, "");
    return true;
  }

  bool WriteTextFileUtf8(const std::string & path, const std::string & content) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE* f = _wfopen(pathW.c_str(), L"w");
    if (!f)
      return false;

    fputs(content.c_str(), f);
    fclose(f);
    return true;
  }

  bool WriteTextFileUtf8(const std::string & path, const ra::strings::StringVector & lines, bool insert_newline_characters) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE* f = _wfopen(pathW.c_str(), L"w");
    if (!f)
      return false;

    for (size_t i = 0; i < lines.size(); i++) {
      const std::string & line = lines[i];
      fputs(line.c_str(), f);

      //add a newline character between each lines
      if (insert_newline_characters) {
        bool isLast = (i == lines.size() - 1);
        if (!isLast) {
          fputs(ra::environment::GetLineSeparator(), f);
        }
      }
    }
    fclose(f);
    return true;
  }

#endif // UTF-8

} //namespace filesystem
} //namespace ra
