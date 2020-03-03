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
#include "rapidassist/filesystem.h"
#include "rapidassist/random.h"
#include "rapidassist/process.h"
#include "rapidassist/unicode.h"

#include <algorithm>  //for std::transform(), sort()
#include <string.h>   //for strdup()
#include <stdlib.h>   //for realpath()

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stat
#define __getcwd _getcwd
#define __chdir _chdir
#define __rmdir _rmdir
#include <direct.h> //for _chdir(), _getcwd()
#include <Windows.h> //for GetShortPathName()
#undef GetEnvironmentVariable
#undef DeleteFile
#undef CreateDirectory
#undef GetCurrentDirectory
#undef CopyFile
#elif __linux__
#define __chdir chdir
#define __getcwd getcwd
#define __rmdir rmdir
#include <unistd.h> //for getcwd()
#include <dirent.h> //for opendir() and closedir()
#include <linux/limits.h> //for PATH_MAX
#endif

namespace ra { namespace filesystem {

  struct greater {
    template<class T>
    bool operator()(T const &a, T const &b) const { return a > b; }
  };

  void NormalizePath(std::string & path) {
    char separator = GetPathSeparator();

    if (separator == '/') {
      //replace invalid path separator
      ra::strings::Replace(path, "\\", "/");
    }
    else if (separator == '\\') {
      //replace invalid path separator
      ra::strings::Replace(path, "/", "\\");
    }

    //make sure the last character of the path is not a separator
    if (path.size() > 0 && path[path.size() - 1] == separator) {
      //remove last character
      path.erase(path.begin() + (path.size() - 1));
    }
  }

  uint32_t GetFileSize(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return 0;

    struct stat sb;
    if (stat(iPath, &sb) == 0) {
      return sb.st_size;
    }

    return 0;
  }

#ifdef _WIN32 // UTF-8
  uint32_t GetFileSizeUtf8(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return 0;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);

    struct _stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      return sb.st_size;
    }

    return 0;
  }
#endif // UTF-8

  uint32_t GetFileSize(FILE * f) {
    if (f == NULL)
      return 0;
    long initPos = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, initPos, SEEK_SET);
    return size;
  }

  std::string GetFilename(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return "";

    std::string directory;
    std::string filename;
    SplitPath(iPath, directory, filename);

    return filename;
  }

  std::string GetFilenameWithoutExtension(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return "";

    std::string filename = ra::filesystem::GetFilename(iPath);
    std::string extension = ra::filesystem::GetFileExtention(iPath);

    //extract filename without extension
    std::string filename_without_extension = filename.substr(0, filename.size() - extension.size());

    //remove last dot of the filename if required
    filename_without_extension = ra::strings::TrimRight(filename_without_extension, '.');

    return filename_without_extension;
  }

  bool FileExists(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    struct stat sb;
    if (stat(iPath, &sb) == 0) {
      if ((sb.st_mode & S_IFREG) == S_IFREG)
        return true;
    }
    return false;
  }

#ifdef _WIN32 // UTF-8
  bool FileExistsUtf8(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);

    struct _stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IFREG) == S_IFREG)
        return true;
    }
    return false;
  }
#endif // UTF-8

  bool HasReadAccess(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    struct stat sb;
    if (stat(iPath, &sb) == 0) {
      if ((sb.st_mode & S_IREAD) == S_IREAD)
        return true;
    }
    return false;
  }

#ifdef _WIN32 // UTF-8
  bool HasReadAccessUtf8(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);

    struct _stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IREAD) == S_IREAD)
        return true;
    }
    return false;
  }
#endif // UTF-8

  bool HasWriteAccess(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    struct stat sb;
    if (stat(iPath, &sb) == 0) {
      if ((sb.st_mode & S_IWRITE) == S_IWRITE)
        return true;
    }
    return false;
  }

#ifdef _WIN32 // UTF-8
  bool HasWriteAccessUtf8(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

    const std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);

    struct _stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IWRITE) == S_IWRITE)
        return true;
    }
    return false;
  }
#endif // UTF-8

  //shared cross-platform code for FindFiles().
  bool processDirectoryEntry(ra::strings::StringVector & oFiles, const char * iDirectoryPath, const std::string & iFilename, bool is_directory, int iDepth, bool use_utf8) {
    //is it a valid item ?
    if (iFilename != "." && iFilename != "..") {
      //build full path
      std::string full_filename = iDirectoryPath;
      NormalizePath(full_filename);
      full_filename.append(GetPathSeparatorStr());
      full_filename.append(iFilename);

      //add this path to the list
      oFiles.push_back(full_filename);

      //should we recurse on directory ?
      if (is_directory && iDepth != 0) {
        //compute new depth
        int sub_depth = iDepth - 1;
        if (sub_depth < -1)
          sub_depth = -1;

        //find children
        bool result = false;
        if (!use_utf8)
          result = FindFiles(oFiles, full_filename.c_str(), sub_depth);
        else
          result = FindFilesUtf8(oFiles, full_filename.c_str(), sub_depth);
        if (!result) {
          return false;
        }
      }
    }

    return true;
  }

  bool FindFiles(ra::strings::StringVector & oFiles, const char * iPath, int iDepth) {
    if (iPath == NULL)
      return false;

#ifdef _WIN32
    //Build a *.* query
    std::string query = iPath;
    NormalizePath(query);
    query << "\\*";

    WIN32_FIND_DATA find_data;
    HANDLE hFind = FindFirstFile(query.c_str(), &find_data);

    if (hFind == INVALID_HANDLE_VALUE)
      return false;

    //process directory entry
    std::string filename = find_data.cFileName;
    bool is_directory = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    bool is_junction = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0); //or JUNCTION, SYMLINK or MOUNT_POINT
    bool result = processDirectoryEntry(oFiles, iPath, filename, is_directory, iDepth, false);
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
    while (FindNextFile(hFind, &find_data)) {
      filename = find_data.cFileName;
      bool is_directory = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
      bool is_junction = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0); //or JUNCTION, SYMLINK or MOUNT_POINT
      bool result = processDirectoryEntry(oFiles, iPath, filename, is_directory, iDepth, false);
      if (!result) {
        //Warning: Current user is not able to browse this directory.
      }
    }
    FindClose(hFind);
    return true;
#elif __linux__
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(iPath)) == NULL) {
      return false;
    }

    while ((dirp = readdir(dp)) != NULL) {
      std::string filename = dirp->d_name;

      bool is_directory = (dirp->d_type == DT_DIR);
      bool result = processDirectoryEntry(oFiles, iPath, filename, is_directory, iDepth, false);
      if (!result) {
        //Warning: Current user is not able to browse this directory.
      }
    }
    closedir(dp);
    return true;
#endif
  }

#ifdef _WIN32 // UTF-8
  bool FindFilesUtf8(ra::strings::StringVector & oFiles, const char * iPath, int iDepth) {
    if (iPath == NULL)
      return false;

    //Build a *.* query
    std::string query = iPath;
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
    bool result = processDirectoryEntry(oFiles, iPath, filename_utf8, is_directory, iDepth, true);
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
      bool result = processDirectoryEntry(oFiles, iPath, filename_utf8, is_directory, iDepth, true);
      if (!result) {
        //Warning: Current user is not able to browse this directory.
      }
    }
    FindClose(hFind);
    return true;
  }
#endif // UTF-8

  bool FindFileFromPaths(const std::string & filename, ra::strings::StringVector & locations) {
    locations.clear();

    //define separator in PATH environment variable
#ifdef _WIN32
    static const char * separator = ";";
#else
    static const char * separator = ":";
#endif

    std::string path_env = ra::environment::GetEnvironmentVariable("PATH");
    if (path_env.empty())
      return false;

    //split each path
    ra::strings::StringVector paths;
    ra::strings::Split(paths, path_env, separator);

    //search within all paths
    bool found = false;
    for (size_t i = 0; i < paths.size(); i++) {
      std::string path = paths[i];

      //Expand the path in case it contains environment variables
      path = ra::environment::Expand(path.c_str());

      //Remove the last path separator (\ or / characters)
      ra::filesystem::NormalizePath(path);

      //append the query filename
      path += ra::filesystem::GetPathSeparatorStr();
      path += filename;

      //look if the file exists
      if (ra::filesystem::FileExists(path.c_str())) {
        //found a possible match for filename
        locations.push_back(path);
        found = true;
      }
    }

    return found;
  }

#ifdef _WIN32 // UTF-8
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
#endif // UTF-8

  std::string FindFileFromPaths(const std::string & filename) {
    ra::strings::StringVector locations;
    bool found = FindFileFromPaths(filename, locations);
    if (!found || locations.size() == 0)
      return "";
    const std::string & first = locations[0];
    return first;
  }

#ifdef _WIN32 // UTF-8
  std::string FindFileFromPathsUtf8(const std::string & filename) {
    ra::strings::StringVector locations;
    bool found = FindFileFromPathsUtf8(filename, locations);
    if (!found || locations.size() == 0)
      return "";
    const std::string & first = locations[0];
    return first;
  }
#endif // UTF-8

  bool DirectoryExists(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

#ifdef _WIN32
    //Note that the current windows implementation of DirectoryExists() uses the _stat() API and the implementation has issues with junctions and symbolink link.
    //For instance, 'C:\Users\All Users\Favorites' exists but 'C:\Users\All Users' don't.
#endif

    struct stat sb;
    if (stat(iPath, &sb) == 0) {
      if ((sb.st_mode & S_IFDIR) == S_IFDIR)
        return true;
    }
    return false;
  }

#ifdef _WIN32 // UTF-8
  bool DirectoryExistsUtf8(const char * iPath) {
    if (iPath == NULL || iPath[0] == '\0')
      return false;

#ifdef _WIN32
    //Note that the current windows implementation of DirectoryExists() uses the _stat() API and the implementation has issues with junctions and symbolink link.
    //For instance, 'C:\Users\All Users\Favorites' exists but 'C:\Users\All Users' don't.
#endif

    std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);

    struct stat sb;
    if (_wstat(pathW.c_str(), &sb) == 0) {
      if ((sb.st_mode & S_IFDIR) == S_IFDIR)
        return true;
    }
    return false;
  }
#endif // UTF-8

  bool CreateDirectory(const char * iPath) {
    if (iPath == NULL)
      return false;

    if (DirectoryExists(iPath))
      return true;

    //directory does not already exists and must be created

    //inspired from https://stackoverflow.com/a/675193
    char *pp;
    char *sp;
    int   status;
    char separator = GetPathSeparator();
#ifdef _WIN32
    char *copypath = _strdup(iPath);
#else
    char *copypath = strdup(iPath);
    static const mode_t mode = 0755;
#endif

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, separator)) != 0) {
      if (sp != pp) {
        /* Neither root nor double slash in path */
        *sp = '\0';
#ifdef _WIN32
        status = _mkdir(copypath);
        if (status == -1 && strlen(copypath) == 2 && copypath[1] == ':') //issue #27
          status = 0; //fix for _mkdir("C:") like
        int errno_copy = errno;
        if (status == -1 && errno == EEXIST)
          status = 0; //File already exist
#else
        status = mkdir(copypath, mode);
#endif
        if (status != 0) {
          //directory already exists?
          if (DirectoryExists(copypath)) {
            status = 0;
          }
        }
        *sp = separator;
      }
      pp = sp + 1;
    }
    if (status == 0) {
#ifdef _WIN32
      status = _mkdir(iPath);
#else
      status = mkdir(iPath, mode);
#endif
    }
    free(copypath);
    return (status == 0);
  }

#ifdef _WIN32 // UTF-8
  bool CreateDirectoryUtf8(const char * iPath) {
    if (iPath == NULL)
      return false;

    if (DirectoryExistsUtf8(iPath))
      return true;

    //directory does not already exists and must be created

    //inspired from https://stackoverflow.com/a/675193
    char *pp;
    char *sp;
    int   status;
    char separator = GetPathSeparator();
    char *copypath = _strdup(iPath);

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
      std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);
      status = _wmkdir(pathW.c_str());
    }
    free(copypath);
    return (status == 0);
  }
#endif // UTF-8

  bool DeleteDirectory(const char * iPath) {
    if (iPath == NULL)
      return false;

    if (!DirectoryExists(iPath))
      return true;

    //directory exists and must be deleted

    //find all files and directories in specified directory
    ra::strings::StringVector files;
    bool found = FindFiles(files, iPath);
    if (!found)
      return false;

    //sort files in reverse order
    //this allows deleting sub-directories and sub-files first
    std::sort(files.begin(), files.end(), greater());

    //process files and directories
    for (size_t i = 0; i < files.size(); i++) {
      const std::string & direntry = files[i];
      if (FileExists(direntry.c_str())) {
        bool deleted = DeleteFile(direntry.c_str());
        if (!deleted)
          return false; //failed to delete file
      }
      else {
        //assume direntry is a directory
        int result = __rmdir(direntry.c_str());
        if (result != 0)
          return false; //failed deleting directory.
      }
    }

    //delete the specified directory
    int result = __rmdir(iPath);
    return (result == 0);
  }

#ifdef _WIN32 // UTF-8
  bool DeleteDirectoryUtf8(const char * iPath) {
    if (iPath == NULL)
      return false;

    if (!DirectoryExistsUtf8(iPath))
      return true;

    //directory exists and must be deleted

    //find all files and directories in specified directory
    ra::strings::StringVector files;
    bool found = FindFilesUtf8(files, iPath);
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
    std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);
    int result = _wrmdir(pathW.c_str());
    return (result == 0);
  }
#endif // UTF-8

  bool DeleteFile(const char * iPath) {
    if (iPath == NULL)
      return false;

    int result = remove(iPath);
    return (result == 0);
  }

#ifdef _WIN32 // UTF-8
  bool DeleteFileUtf8(const char * iPath) {
    if (iPath == NULL)
      return false;

    std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);
    int result = _wremove(pathW.c_str());
    return (result == 0);
  }
#endif // UTF-8

  std::string GetTemporaryFileName() {
    //get positive random value
    int value = random::GetRandomInt();
    if (value < 0)
      value *= -1;

    //RAND_MAX expected to get as high as 32768
    //5 characters is required for printing the value

    char str[1024];
    sprintf(str, "random.%05d.tmp", value);

    return std::string(str);
  }

  std::string GetTemporaryFilePath() {
    std::string temp_dir = GetTemporaryDirectory();
    std::string rnd_path = temp_dir + GetPathSeparator() + GetTemporaryFileName();
    return rnd_path;
  }

#ifdef _WIN32 // UTF-8
  std::string GetTemporaryFilePathUtf8() {
    std::string temp_dir = GetTemporaryDirectoryUtf8();
    std::string rnd_path = temp_dir + GetPathSeparator() + GetTemporaryFileName();
    return rnd_path;
  }
#endif // UTF-8

  std::string GetTemporaryDirectory() {
#ifdef _WIN32
    std::string temp = environment::GetEnvironmentVariable("TEMP");
#elif __linux__
    std::string temp = "/tmp";
#endif
    return temp;
  }

  std::string GetParentPath(const std::string & iPath) {
    std::string parent;

    std::size_t offset = iPath.find_last_of("/\\");
    if (offset != std::string::npos) {
      //found
      parent = iPath.substr(0, offset);
    }

    return parent;
  }

  std::string getShortPathFormEstimation(const std::string & iPath) {
    std::string short_path;

    std::vector<std::string> path_elements;
    SplitPath(iPath, path_elements);
    for (size_t i = 0; i < path_elements.size(); i++) {
      const std::string & element = path_elements[i];
      if (element.size() > 12 || element.find(' ') != std::string::npos) {
        std::string element83 = element;
        std::string ext = GetFileExtention(element);
        strings::Replace(element83, (std::string(".") + ext), ""); //remove extension from filename
        strings::Replace(ext, " ", ""); //remove spaces in extension
        ext = ext.substr(0, 3); //truncate file extension
        strings::Replace(element83, " ", ""); //remove spaces
        element83 = element83.substr(0, 6); //truncate file name
        element83.append("~1");
        if (!ext.empty()) {
          element83.append(".");
          element83.append(ext);
        }

        //uppercase everything
        std::transform(element83.begin(), element83.end(), element83.begin(), ::toupper);

        //add to short_path
        if (!short_path.empty())
          short_path.append("\\");
        short_path.append(element83);
      }
      else {
        if (!short_path.empty())
          short_path.append("\\");
        short_path.append(element);
      }
    }

    return short_path;
  }

  std::string getShortPathFormWin32(const std::string & iPath) {
    std::string short_path;

#ifdef _WIN32
    // First obtain the size needed by passing NULL and 0.
    long length = GetShortPathName(iPath.c_str(), NULL, 0);
    if (length == 0)
      return "";

    // Dynamically allocate the correct size
    // (terminating null char was included in length)
    char * buffer = new char[length];

    // Now simply call again using same long path.
    length = GetShortPathName(iPath.c_str(), buffer, length);
    if (length == 0)
      return "";

    short_path = buffer;

    delete[] buffer;
#endif

    return short_path;
  }

  std::string GetShortPathForm(const std::string & iPath) {
#ifdef WIN32
    if (FileExists(iPath.c_str()) || DirectoryExists(iPath.c_str())) {
      //file must exist to use WIN32 api
      return getShortPathFormWin32(iPath);
    }
    else {
      return getShortPathFormEstimation(iPath);
    }
#elif __linux__
    //no such thing as short path form in unix
    return getShortPathFormEstimation(iPath);
#endif
  }

  void SplitPath(const std::string & iPath, std::string & oDirectory, std::string & oFilename) {
    oDirectory = "";
    oFilename = "";

    std::size_t offset = iPath.find_last_of("/\\");
    if (offset != std::string::npos) {
      //found
      oDirectory = iPath.substr(0, offset);
      oFilename = iPath.substr(offset + 1);
    }
    else {
      oFilename = iPath;
    }
  }

  void SplitPath(const std::string & iPath, std::vector<std::string> & oElements) {
    oElements.clear();
    std::string s = iPath;
    std::string accumulator;
    for (unsigned int i = 0; i < s.size(); i++) {
      const char & c = s[i];
      if ((c == '/' || c == '\\')) {
        if (accumulator.size() > 0) {
          oElements.push_back(accumulator);
          accumulator = "";
        }
      }
      else
        accumulator += c;
    }
    if (accumulator.size() > 0) {
      oElements.push_back(accumulator);
      accumulator = "";
    }
  }

  std::string MakeRelativePath(const std::string & base_path, const std::string & test_path) {
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();

    ra::strings::StringVector base_path_parts = ra::strings::Split(base_path, path_separator.c_str());
    ra::strings::StringVector test_path_parts = ra::strings::Split(test_path, path_separator.c_str());

    bool have_common_base = false; //true if base_path and test_path share a common base

    //remove beginning of both parts while they match
    while (base_path_parts.size() >= 1 && test_path_parts.size() >= 1 && base_path_parts[0] == test_path_parts[0]) {
      //remove first
      base_path_parts.erase(base_path_parts.begin());
      test_path_parts.erase(test_path_parts.begin());

      have_common_base = true;
    }

    if (have_common_base) {
      //resolve:
      //  from base_path,
      //  go up as many element in base_path_parts,
      //  then go through test_path_parts

      for (size_t i = 0; i < base_path_parts.size(); i++) {
        base_path_parts[i] = "..";
      }

      std::string new_base_path = ra::strings::Join(base_path_parts, path_separator.c_str());
      std::string new_test_path = ra::strings::Join(test_path_parts, path_separator.c_str());

      //build relative path
      std::string relative_path = new_base_path;
      if (!relative_path.empty())
        relative_path.append(path_separator);
      relative_path.append(new_test_path);
      return relative_path;
    }

    //failed making path relative
    return "";
  }

  char GetPathSeparator() {
#ifdef _WIN32
    return '\\';
#elif __linux__
    return '/';
#endif
  }

  const char * GetPathSeparatorStr() {
#ifdef _WIN32
    return "\\";
#elif __linux__
    return "/";
#endif
  }

  std::string GetCurrentDirectory() {
    return std::string(__getcwd(NULL, 0));
  }

#ifdef _WIN32 // UTF-8
  std::string GetCurrentDirectoryUtf8() {
    std::wstring curdirW = _wgetcwd(NULL, 0);
    std::string curdir = ra::unicode::UnicodeToUtf8(curdirW);
    return curdir;
  }
#endif // UTF-8

  std::string GetFileExtention(const std::string & iPath) {
    //extract filename from path to prevent
    //reading a directory's extension
    std::string directory;
    std::string filename;
    SplitPath(iPath, directory, filename);

    std::string extension;
    std::size_t offset = filename.find_last_of(".");
    if (offset != std::string::npos) {
      //found
      //name = filename.substr(0,offset);
      extension = filename.substr(offset + 1);
    }

    return extension;
  }

  std::string GetUserFriendlySize(uint64_t iBytesSize) {
    static const uint64_t kb_limit = 1024;
    static const uint64_t mb_limit = kb_limit * 1000;
    static const uint64_t gb_limit = 1024 * mb_limit;
    static const uint64_t tb_limit = 1024 * gb_limit;

    FileSizeEnum preferedUnit = BYTES;

    if (iBytesSize < kb_limit) {
      //bytes
    }
    else if (iBytesSize < mb_limit) {
      preferedUnit = KILOBYTES;
    }
    else if (iBytesSize < gb_limit) {
      preferedUnit = MEGABYTES;
    }
    else if (iBytesSize < tb_limit) {
      preferedUnit = GIGABYTES;
    }
    else {
      preferedUnit = TERABYTES;
    }

    return GetUserFriendlySize(iBytesSize, preferedUnit);
  }

  std::string GetUserFriendlySize(uint64_t iBytesSize, FileSizeEnum iUnit) {
    static const uint64_t digits_precision = 100;
    static const uint64_t factor = 1024;
    static const uint64_t kb_precision = 1;
    static const uint64_t mb_precision = 1024 * kb_precision;
    static const uint64_t gb_precision = 1024 * mb_precision;
    static const uint64_t tb_precision = 1024 * gb_precision;

    std::string friendly_size;

    //Convert iSize to a formatted_size
    double formatted_size = 0.0;
    switch (iUnit) {
    case BYTES:
      formatted_size = double(iBytesSize);
      break;
    case KILOBYTES:
      formatted_size = double(((iBytesSize*digits_precision) / factor) / kb_precision) / double(digits_precision);
      break;
    case MEGABYTES:
      formatted_size = double(uint64_t(uint64_t(iBytesSize / factor)*digits_precision) / mb_precision) / double(digits_precision);
      break;
    case GIGABYTES:
      formatted_size = double(uint64_t(uint64_t(iBytesSize / factor)*digits_precision) / gb_precision) / double(digits_precision);
      break;
    case TERABYTES:
      formatted_size = double(uint64_t(uint64_t(iBytesSize / factor)*digits_precision) / tb_precision) / double(digits_precision);
      break;
    };

    //Add formatted_size to friendly_size
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%.2f", formatted_size);
    friendly_size = buffer;

    //Append unit descrition to friendly_size
    switch (iUnit) {
    case BYTES:
    {
      friendly_size = strings::ToString(iBytesSize);
      friendly_size += " bytes";
    };
    break;
    case KILOBYTES:
    {
      friendly_size += " KB";
    };
    break;
    case MEGABYTES:
    {
      friendly_size += " MB";
    };
    break;
    case GIGABYTES:
    {
      friendly_size += " GB";
    };
    break;
    case TERABYTES:
    {
      friendly_size += " TB";
    };
    break;
    };

    return friendly_size;
  }

  uint64_t GetFileModifiedDate(const std::string & iPath) {
    struct stat result;
    uint64_t mod_time = 0;
    if (stat(iPath.c_str(), &result) == 0) {
      mod_time = result.st_mtime;
    }
    return mod_time;
  }

#ifdef _WIN32 // UTF-8
  uint64_t GetFileModifiedDateUtf8(const std::string & iPath) {
    struct stat result;
    uint64_t mod_time = 0;
    std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);
    if (_wstat(pathW.c_str(), &result) == 0) {
      mod_time = result.st_mtime;
    }
    return mod_time;
  }
#endif // UTF-8

#ifdef _WIN32
  inline bool isDriveLetter(char c) {
    if (c >= 'a' && c <= 'z')
      return true;
    if (c >= 'A' && c <= 'Z')
      return true;
    return false;
  }
#endif
  bool IsAbsolutePath(const std::string & iPath) {
    if (iPath.length() > 0 && iPath[0] == ra::filesystem::GetPathSeparator())
      return true; //this is also true for `\\server\shared` path on Windows.
#ifdef _WIN32
    if (iPath.length() > 2 && iPath[1] == ':' && iPath[2] == '\\' && isDriveLetter(iPath[0]))
      return true; // For `C:\` format on Windows
#endif
    return false;
  }

  bool IsRootDirectory(const std::string & iPath) {
    if (iPath == "/")
      return true;
#ifdef _WIN32
    if (iPath.length() == 3 && iPath[1] == ':' && iPath[2] == '\\' && isDriveLetter(iPath[0]))
      return true; // For `C:\` format on Windows
#endif
    return false;
  }

  size_t find(const ra::strings::StringVector & items, size_t offset, const std::string & value) {
    for (size_t i = offset; i < items.size(); i++) {
      const std::string & str = items[i];
      if (str == value)
        return i;
    }
    return std::string::npos;
  }

  std::string ResolvePath(const std::string & iPath) {
    std::string output;
    //#ifdef _WIN32
    //      char absolutePath[_MAX_PATH+1] = "";
    //      if ( _fullpath(absolutePath, iPath.c_str(), _MAX_PATH) )
    //      {
    //        output = absolutePath;
    //      }
    //#endif

    static const std::string CURRENT_DIRECTORY = ".";
    static const std::string PREVIOUS_DIRECTORY = "..";
    std::string path = iPath;

    //remove `/./` path elements
    {
      std::string pattern;
      pattern << ra::filesystem::GetPathSeparatorStr() << CURRENT_DIRECTORY << ra::filesystem::GetPathSeparatorStr();
      ra::strings::Replace(path, pattern, ra::filesystem::GetPathSeparatorStr());

      //look for a path that ends with /. or \.
      pattern.clear();
      pattern << ra::filesystem::GetPathSeparatorStr() << ".";
      if (path.size() >= pattern.size() && pattern == &path[path.size() - pattern.size()]) {
        //remove the ending
        size_t offset = path.size() - pattern.size();
        path.erase(offset, pattern.size());
      }
    }

    //remove `/../` path elements
    {
      //split by path separator
      ra::strings::StringVector elements;
      ra::strings::Split(elements, path, ra::filesystem::GetPathSeparatorStr());

      //search for /..
      size_t offset = 0; //current index of the element where we are searching
      size_t index = find(elements, offset, PREVIOUS_DIRECTORY);
      while (index != std::string::npos) {
        if (index == 0) {
          //there is nothing we can do with this element
          offset = index + 1; //continue resolving from the next element
        }
        else {
          //get previous element
          const std::string & previous_element = elements[index - 1];

          if (previous_element == PREVIOUS_DIRECTORY) {
            //there is nothing we can do with this element
            offset = index + 1; //continue resolving from the next element
          }
          else {
            //remove the `..` element
            elements.erase(elements.begin() + index);

            //should we remove the element before?
            //Note:
            //On Windows, the root element is defined by the string "C:" (without the \ character). The \ character must be
            //added to `previous_element` to be properly detected by `IsRootDirectory()`.
            //On Linux, the root element is defined by an empty string. This is because an absolute path starts with a
            //separator which creates an empty string element when calling `ra::strings::Split()`.
            if (IsRootDirectory(previous_element + ra::filesystem::GetPathSeparatorStr())) {
              //one cannot walk down past the root

              //keep same offset. The next search will occurs where the deleted `..` element was.
            }
            else {
              //erase previous element
              elements.erase(elements.begin() + (index - 1));

              offset = index - 1; //The next search will occurs where `previous_element` was.
            }
          }
        }

        //search again
        index = find(elements, offset, PREVIOUS_DIRECTORY);
      }

      //join the string back again
      path = ra::strings::Join(elements, ra::filesystem::GetPathSeparatorStr());
    }

    output = path;

    return output;
  }

  std::string GetPathBasedOnCurrentProcess(const std::string & iPath) {
    if (IsAbsolutePath(iPath))
      return iPath;

    std::string dir = ra::process::GetCurrentProcessDir();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(iPath);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }

#ifdef _WIN32 // UTF-8
  std::string GetPathBasedOnCurrentProcessUtf8(const std::string & iPath) {
    if (IsAbsolutePath(iPath))
      return iPath;

    std::string dir = ra::process::GetCurrentProcessDirUtf8();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(iPath);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }
#endif // UTF-8

  std::string GetPathBasedOnCurrentDirectory(const std::string & iPath) {
    if (IsAbsolutePath(iPath))
      return iPath;

    std::string dir = ra::filesystem::GetCurrentDirectory();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(iPath);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }

#ifdef _WIN32 // UTF-8
  std::string GetPathBasedOnCurrentDirectoryUtf8(const std::string & iPath) {
    if (IsAbsolutePath(iPath))
      return iPath;

    std::string dir = ra::filesystem::GetCurrentDirectoryUtf8();
    ra::filesystem::NormalizePath(dir); //remove last / or \ character if any API used return an unexpected value

    std::string tmp_path;
    tmp_path.append(dir);
    tmp_path.append(ra::filesystem::GetPathSeparatorStr());
    tmp_path.append(iPath);

    std::string resolved = ResolvePath(tmp_path);

    return resolved;
  }
#endif // UTF-8

  bool copyFileInternal(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor, ProgressReportCallback progress_function, bool force_win32_utf8) {
    size_t file_size = ra::filesystem::GetFileSize(source_path.c_str());

    FILE* fin = NULL;
    if (!force_win32_utf8)
      fin = fopen(source_path.c_str(), "rb");
#ifdef _WIN32 // UTF-8
    if (force_win32_utf8)
    {
      std::wstring source_path_w = ra::unicode::Utf8ToUnicode(source_path);
      fin = _wfopen(source_path_w.c_str(), L"rb");
    }
#endif // UTF-8

    if (!fin)
      return false;

    FILE* fout = NULL;
    if (!force_win32_utf8)
      fout = fopen(destination_path.c_str(), "wb");
#ifdef _WIN32 // UTF-8
    if (force_win32_utf8)
    {
      std::wstring destination_w = ra::unicode::Utf8ToUnicode(destination_path);
      fout = _wfopen(destination_w.c_str(), L"rb");
    }
#endif // UTF-8
    
    if (!fout) {
      fclose(fin);
      return false;
    }

    //publish progress
    double progress = 0.0;
    if (progress_functor)
      progress_functor->OnProgressReport(progress);
    if (progress_function)
      progress_function(progress);

    const size_t buffer_size = 100 * 1024; //100k memory buffer
    uint8_t buffer[buffer_size];

    size_t copied_size = 0;

    while (!feof(fin)) {
      size_t size_readed = fread(buffer, 1, buffer_size, fin);
      if (size_readed) {
        size_t size_writen = fwrite(buffer, 1, size_readed, fout);
        copied_size += size_writen;

        //publish progress
        progress = double(copied_size) / double(file_size);
        if (progress_functor)
          progress_functor->OnProgressReport(progress);
        if (progress_function)
          progress_function(progress);
      }
    }

    fclose(fin);
    fclose(fout);

    bool success = (file_size == copied_size);

    if (success && progress < 1.0) //if 100% progress not already sent
    {
      //publish progress
      progress = 1.0;
      if (progress_functor)
        progress_functor->OnProgressReport(progress);
      if (progress_function)
        progress_function(progress);
    }

    return success;
  }

  bool CopyFile(const std::string & source_path, const std::string & destination_path) {
    return copyFileInternal(source_path, destination_path, NULL, NULL, false);
  }

  bool CopyFile(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor) {
    return copyFileInternal(source_path, destination_path, progress_functor, NULL, false);
  }

  bool CopyFile(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function) {
    return copyFileInternal(source_path, destination_path, NULL, progress_function, false);
  }

#ifdef _WIN32 // UTF-8
  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path) {
    return copyFileInternal(source_path, destination_path, NULL, NULL, true);
  }

  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, IProgressReport * progress_functor) {
    return copyFileInternal(source_path, destination_path, progress_functor, NULL, true);
  }

  bool CopyFileUtf8(const std::string & source_path, const std::string & destination_path, ProgressReportCallback progress_function) {
    return copyFileInternal(source_path, destination_path, NULL, progress_function, true);
  }
#endif // UTF-8

  bool PeekFile(const std::string & path, size_t size, std::string & data) {
    //static const std::string EMPTY;
    data.clear();

    //validate if file exists
    if (!ra::filesystem::FileExists(path.c_str()))
      return false;

    //allocate a buffer which can hold the data of the peek size
    size_t file_size = ra::filesystem::GetFileSize(path.c_str());
    size_t max_read_size = (file_size < size ? file_size : size);

    //validates empty files 
    if (max_read_size == 0)
      return true;

    FILE * f = fopen(path.c_str(), "rb");
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

#ifdef _WIN32 // UTF-8
  bool PeekFileUtf8(const std::string & path, size_t size, std::string & data) {
    //static const std::string EMPTY;
    data.clear();

    //validate if file exists
    if (!ra::filesystem::FileExistsUtf8(path.c_str()))
      return false;

    //allocate a buffer which can hold the data of the peek size
    size_t file_size = ra::filesystem::GetFileSizeUtf8(path.c_str());
    size_t max_read_size = (file_size < size ? file_size : size);

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
#endif // UTF-8

  bool ReadFile(const std::string & path, std::string & data) {
    //validate if file exists
    if (!ra::filesystem::FileExists(path.c_str()))
      return false;

    uint32_t file_size = ra::filesystem::GetFileSize(path.c_str());

    //validates empty files 
    if (file_size == 0)
      return true;

    bool readed = PeekFile(path, file_size, data);
    return readed;
  }

#ifdef _WIN32 // UTF-8
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
#endif // UTF-8

  bool WriteFile(const std::string & path, const std::string & data) {
    FILE * f = fopen(path.c_str(), "wb");
    if (!f)
      return false;

    size_t size_write = fwrite(data.c_str(), 1, data.size(), f);

    fclose(f);

    bool success = (data.size() == size_write);
    return success;
  }

#ifdef _WIN32 // UTF-8
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
#endif // UTF-8

  bool FileReplace(const std::string & path, const std::string & oldvalue, const std::string & newvalue) {
    std::string data;
    if (!ReadFile(path, data))
      return false;

    int num_finding = ra::strings::Replace(data, oldvalue, newvalue);

    //does the file was modified?
    if (num_finding) {
      //yes, write modifications to the file
      if (!WriteFile(path, data))
        return false;
    }

    return true;
  }

#ifdef _WIN32 // UTF-8
  bool FileReplaceUtf8(const std::string & path, const std::string & oldvalue, const std::string & newvalue) {
    std::string data;
    if (!ReadFileUtf8(path, data))
      return false;

    int num_finding = ra::strings::Replace(data, oldvalue, newvalue);

    //does the file was modified?
    if (num_finding) {
      //yes, write modifications to the file
      if (!WriteFileUtf8(path, data))
        return false;
    }

    return true;
  }
#endif // UTF-8

  bool ReadTextFile(const std::string & path, ra::strings::StringVector & lines, bool trim_newline_characters) {
    lines.clear();

    static const int BUFFER_SIZE = 10240;
    char buffer[BUFFER_SIZE];

    FILE* f = fopen(path.c_str(), "r");
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

#ifdef _WIN32 // UTF-8
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
#endif // UTF-8

  bool ReadTextFile(const std::string & path, std::string & content) {
    ra::strings::StringVector lines;
    bool success = ReadTextFile(path.c_str(), lines, false);
    if (!success)
      return false;

    //merge all lines (including newline characters)
    content = ra::strings::Join(lines, "");
    return true;
  }

#ifdef _WIN32 // UTF-8
  bool ReadTextFileUtf8(const std::string & path, std::string & content) {
    ra::strings::StringVector lines;
    bool success = ReadTextFileUtf8(path.c_str(), lines, false);
    if (!success)
      return false;

    //merge all lines (including newline characters)
    content = ra::strings::Join(lines, "");
    return true;
  }
#endif // UTF-8

  bool WriteTextFile(const std::string & path, const std::string & content) {
    FILE* f = fopen(path.c_str(), "w");
    if (!f)
      return false;

    fputs(content.c_str(), f);
    fclose(f);
    return true;
  }

#ifdef _WIN32 // UTF-8
  bool WriteTextFileUtf8(const std::string & path, const std::string & content) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE* f = _wfopen(pathW.c_str(), L"w");
    if (!f)
      return false;

    fputs(content.c_str(), f);
    fclose(f);
    return true;
  }
#endif // UTF-8

  bool WriteTextFile(const std::string & path, const ra::strings::StringVector & lines, bool insert_newline_characters) {
    FILE* f = fopen(path.c_str(), "w");
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

#ifdef _WIN32 // UTF-8
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
