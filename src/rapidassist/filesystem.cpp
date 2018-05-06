#include "environment.h"
#include "filesystem.h"

#include <algorithm> //for std::transform(), sort()

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stat
#define __getcwd _getcwd
#define __chdir _chdir
#define __rmdir _rmdir
#include <direct.h> //for _chdir(), _getcwd()
#include <Windows.h> //for GetShortPathName()
#elif __linux__
#define __chdir chdir
#define __getcwd getcwd
#define __rmdir rmdir
#include <unistd.h> //for getcwd()
#include <dirent.h> //for opendir() and closedir()
#endif

namespace ra
{

  namespace filesystem
  {
    struct greater
    {
      template<class T>
      bool operator()(T const &a, T const &b) const { return a > b; }
    };

    void normalizePath(std::string & path)
    {
      char separator = getPathSeparator();

      if (separator == '/')
      {
        //replace invalid path separator
        ra::strings::strReplace(path, "\\", "/");
      }
      else if (separator == '\\')
      {
        //replace invalid path separator
        ra::strings::strReplace(path, "/", "\\");
      }

      //make sure the last character of the path is not a separator
      if (path.size() > 0 && path[path.size()-1] == separator)
      {
        //remove last character
        path.erase( path.begin()+(path.size()-1) );
      }
    }

    uint32_t getFileSize(const char * iPath)
    {
      if (iPath == NULL || iPath[0] == '\0')
        return 0;

      FILE * f = fopen(iPath, "rb");
      if (!f)
        return 0;
      uint32_t size = getFileSize(f);
      fclose(f);
      return size;
    }

    uint32_t getFileSize(FILE * f)
    {
      if (f == NULL)
        return 0;
      long initPos = ftell(f);
      fseek(f, 0, SEEK_END);
      long size = ftell(f);
      fseek(f, initPos, SEEK_SET);
      return size;
    }

    std::string getFilename(const char * iPath)
    {
      if (iPath == NULL || iPath[0] == '\0')
        return std::string();

      std::string folder;
      std::string filename;
      splitPath(iPath, folder, filename);

      return filename;
    }

    bool fileExists(const char * iPath)
    {
      if (iPath == NULL || iPath[0] == '\0')
        return false;

      FILE * f = fopen(iPath, "rb");
      if (!f)
        return false;
      fclose(f);
      return true;
    }

    inline bool isCurrentFolder(const std::string & iPath)
    {
      return iPath == ".";
    }

    inline bool isParentFolder(const std::string & iPath)
    {
      return iPath == "..";
    }

    bool processDirectoryEntry(ra::strings::StringVector & oFiles, const char * iFolderPath, const std::string & iFilename, bool isFolder, int iDepth)
    {
      //is it a valid item ?
      if (!isCurrentFolder(iFilename) && !isParentFolder(iFilename))
      {
        //build full path
        std::string fullFilename;
        fullFilename << iFolderPath << getPathSeparatorStr() << iFilename;
        oFiles.push_back(fullFilename);

        //should we recurse on folder ?
        if (isFolder && iDepth != 0)
        {
          //find children
          bool result = findFiles(oFiles, fullFilename.c_str(), iDepth-1);
          if (!result)
          {
            return false;
          }
        }
      }

      return true;
    }

    bool findFiles(ra::strings::StringVector & oFiles, const char * iPath, int iDepth)
    {
      if (iPath == NULL)
        return false;

      if (!folderExists(iPath))
        return false;

#ifdef _WIN32
      //Build a *.* query
      std::string query;
      query << iPath << "\\*";

      WIN32_FIND_DATA findDataStruct;
      HANDLE hFind = FindFirstFile(query.c_str(), &findDataStruct);

      if(hFind == INVALID_HANDLE_VALUE)
        return false;

      //process directory entry
      std::string filename = findDataStruct.cFileName;
      bool isFolder = ((findDataStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
      bool result = processDirectoryEntry(oFiles, iPath, filename, isFolder, iDepth);
      if (!result)
      {
        FindClose(hFind);
        return false;
      }

      //next files in folder
      while (FindNextFile(hFind, &findDataStruct))
      {
        filename = findDataStruct.cFileName;
        bool isFolder = ((findDataStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
        bool result = processDirectoryEntry(oFiles, iPath, filename, isFolder, iDepth);
        if (!result)
        {
          FindClose(hFind);
          return false;
        }
      }
      FindClose(hFind);
      return true;
#elif __linux__
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL)
    {
      return false;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
      std::string filename = dirp->d_name;

      bool isFolder = (dirp->d_type == DT_DIR);
      bool result = processDirectoryEntry(oFiles, iPath, filename, isFolder, iDepth);
      if (!result)
      {
        closedir(dp);
        return false;
      }
    }
    closedir(dp);
    return true;
#endif
    }

    bool folderExists(const char * iPath)
    {
      if (iPath == NULL || iPath[0] == '\0')
        return false;

      std::string localFolder = getCurrentFolder();
      bool success = (__chdir(iPath) == 0);
      if (success)
        success = (__chdir(localFolder.c_str()) == 0);
      return success;
    }

    bool createFolder(const char * iPath)
    {
      if (iPath == NULL)
        return false;

      if (folderExists(iPath))
        return true;

      //folder does not already exists and must be created

      //inspired from https://stackoverflow.com/a/675193
      char *pp;
      char *sp;
      int   status;
      char separator = getPathSeparator();
#ifdef _WIN32
      char *copypath = _strdup(iPath);
#else
      char *copypath = strdup(iPath);
      static const mode_t mode = 755;
#endif

      status = 0;
      pp = copypath;
      while (status == 0 && (sp = strchr(pp, separator)) != 0)
      {
        if (sp != pp)
        {
          /* Neither root nor double slash in path */
          *sp = '\0';
#ifdef _WIN32
          status = _mkdir(copypath);
#else
          status = mkdir(copypath, mode);
#endif
          if (status != 0)
          {
            //folder already exists?
            if (folderExists(copypath))
            {
              status = 0;
            }
          }
          *sp = separator;
        }
        pp = sp + 1;
      }
      if (status == 0)
      {
#ifdef _WIN32
        status = _mkdir(iPath);
#else
        status = mkdir(iPath, mode);
#endif
      }
      free(copypath);
      return (status == 0);      
    }

    bool deleteFolder(const char * iPath)
    {
      if (iPath == NULL)
        return false;

      if (!folderExists(iPath))
        return true;

      //folder exists and must be deleted

      //find all files and folders in specified directory
      ra::strings::StringVector files;
      bool foundFiles = findFiles(files, iPath);
      if (!foundFiles)
        return false;

      //soft files in reverse order
      //this allows deleting sub-folders and sub-files first
      std::sort(files.begin(), files.end(), greater());

      //process files and folders
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & direntry = files[i];
        if (fileExists(direntry.c_str()))
        {
          bool deleted = deleteFile(direntry.c_str());
          if (!deleted)
            return false; //failed to delete file
        }
        else
        {
          //assume direntry is a folder
          int result = __rmdir(direntry.c_str());
          if (result != 0)
            return false; //failed deleting folder.
        }
      }

      //delete the specified folder
      int result = __rmdir(iPath);
      return (result == 0);
    }

    bool deleteFile(const char * iPath)
    {
      if (iPath == NULL)
        return false;

      if (!fileExists(iPath))
        return true;

      //file exists and must be deleted

      int result = remove(iPath);
      return (result == 0);
    }

    std::string getTemporaryFileName()
    {
      //get positive random value
      int value = rand();
      if (value < 0)
        value *= -1;

      //RAND_MAX expected to get as high as 32768
      //5 characters is required for printing the value

      char str[1024];
      sprintf(str, "random.%05d.tmp", value);

      return std::string(str);
    }

    std::string getTemporaryFilePath()
    {
#ifdef _WIN32
      std::string temp = environment::getEnvironmentVariable("TEMP");
#elif __linux__
      std::string temp = "/tmp";
#endif
      std::string rndpath = temp + getPathSeparator() + getTemporaryFileName();
      return rndpath;
    }

    std::string getParentPath(const std::string & iPath)
    {
      std::string parent;

      std::size_t offset = iPath.find_last_of("/\\");
      if (offset != std::string::npos)
      {
        //found
        parent = iPath.substr(0,offset);
      }

      return parent;
    }

    std::string getShortPathFormEstimation(const std::string & iPath)
    {
      std::string shortPath;

      std::vector<std::string> pathElements;
      splitPath(iPath, pathElements);
      for(size_t i=0; i<pathElements.size(); i++)
      {
        const std::string & element = pathElements[i];
        if (element.size() > 12 || element.find(' ') != std::string::npos)
        {
          std::string element83 = element;
          std::string ext = getFileExtention(element);
          strings::strReplace(element83, (std::string(".")+ext).c_str(), ""); //remove extension from filename
          strings::strReplace(ext, " ", ""); //remove spaces in extension
          ext = ext.substr(0, 3); //truncate file extension
          strings::strReplace(element83, " ", ""); //remove spaces
          element83 = element83.substr(0, 6); //truncate file name
          element83.append("~1");
          if (!ext.empty())
          {
            element83.append(".");
            element83.append(ext);
          }

          //uppercase everything
          std::transform(element83.begin(), element83.end(), element83.begin(), ::toupper);

          //add to shortPath
          if (!shortPath.empty())
            shortPath.append("\\");
          shortPath.append(element83);
        }
        else
        {
          if (!shortPath.empty())
            shortPath.append("\\");
          shortPath.append(element);
        }
      }

      return shortPath;
    }

    std::string getShortPathFormWin32(const std::string & iPath)
    {
      std::string shortPath;

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

      shortPath = buffer;

      delete [] buffer;
#endif

      return shortPath;
    }

    std::string getShortPathForm(const std::string & iPath)
    {
#ifdef WIN32
      if (fileExists(iPath.c_str()) || folderExists(iPath.c_str()))
      {
        //file must exist to use WIN32 api
        return getShortPathFormWin32(iPath);
      }
      else
      {
        return getShortPathFormEstimation(iPath);
      }
#elif __linux__
      //no such thing as short path form in unix
      return getShortPathFormEstimation(iPath);
#endif
    }

    void splitPath(const std::string & iPath, std::string & oFolder, std::string & oFilename)
    {
      oFolder = "";
      oFilename = "";

      std::size_t offset = iPath.find_last_of("/\\");
      if (offset != std::string::npos)
      {
        //found
        oFolder = iPath.substr(0,offset);
        oFilename = iPath.substr(offset+1);
      }
      else
      {
        oFilename = iPath;
      }
    }

    void splitPath(const std::string & iPath, std::vector<std::string> & oElements)
    {
      oElements.clear();
      std::string s = iPath;
      std::string accumulator;
      for(unsigned int i = 0; i<s.size(); i++)
      {
        const char & c = s[i];
        if ((c == '/' || c == '\\'))
        {
          if (accumulator.size() > 0)
          {
            oElements.push_back(accumulator);
            accumulator = "";
          }
        }
        else
          accumulator += c;
      }
      if (accumulator.size() > 0)
      {
        oElements.push_back(accumulator);
        accumulator = "";
      }
    }

    char getPathSeparator()
    {
#ifdef _WIN32
      return '\\';
#elif __linux__
      return '/';
#endif
    }

    const char * getPathSeparatorStr()
    {
#ifdef _WIN32
      return "\\";
#elif __linux__
      return "/";
#endif
    }

    std::string getCurrentFolder()
    {
      return std::string(__getcwd(NULL, 0));
    }

    std::string getFileExtention(const std::string & iPath)
    {
      //extract filename from path to prevent
      //reading a folder's extension
      std::string folder;
      std::string filename;
      splitPath(iPath, folder, filename);

      std::string extension;
      std::size_t offset = filename.find_last_of(".");
      if (offset != std::string::npos)
      {
        //found
        //name = filename.substr(0,offset);
        extension = filename.substr(offset+1);
      }

      return extension;
    }

    std::string getUserFriendlySize(uint64_t iBytesSize)
    {
      static const uint64_t kbLimit = 1024;
      static const uint64_t mbLimit = kbLimit*1000;
      static const uint64_t gbLimit = 1024*mbLimit;
      static const uint64_t tbLimit = 1024*gbLimit;

      FileSizeEnum preferedUnit = Bytes;

      if (iBytesSize < kbLimit)
      {
        //bytes
      }
      else if (iBytesSize < mbLimit)
      {
        preferedUnit = Kilobytes;
      }
      else if (iBytesSize < gbLimit)
      {
        preferedUnit = Megabytes;
      }
      else if (iBytesSize < tbLimit)
      {
        preferedUnit = Gigabytes;
      }
      else
      {
        preferedUnit = Terabytes;
      }

      return getUserFriendlySize(iBytesSize, preferedUnit);
    }

    std::string getUserFriendlySize(uint64_t iBytesSize, FileSizeEnum iUnit)
    {
      static const uint64_t digitsPrecision = 100;
      static const uint64_t factor = 1024;
      static const uint64_t kbPrecision = 1;
      static const uint64_t mbPrecision = 1024*kbPrecision;
      static const uint64_t gbPrecision = 1024*mbPrecision;
      static const uint64_t tbPrecision = 1024*gbPrecision;

      std::string friendlySize;

      //Convert iSize to a formattedSize
      //double unitPower = double(iUnit.getValue());
      //double multiplicator = pow(1024.0, unitPower);
      //double sizeInSpecifiedUnit = double(iSize)/multiplicator;
      //double formattedSize = double(uint64_t(sizeInSpecifiedUnit * 100.0)) / 100.0;
      double formattedSize = 0.0;
      switch(iUnit)
      {
      case Bytes:
        formattedSize = double( iBytesSize );
        break;
      case Kilobytes:
        formattedSize = double( ((iBytesSize*digitsPrecision)/factor)/kbPrecision )/double(digitsPrecision);
        break;
      case Megabytes:
        formattedSize = double( uint64_t(uint64_t(iBytesSize/factor)*digitsPrecision)/mbPrecision )/double(digitsPrecision);
        break;
      case Gigabytes:
        formattedSize = double( uint64_t(uint64_t(iBytesSize/factor)*digitsPrecision)/gbPrecision )/double(digitsPrecision);
        break;
      case Terabytes:
        formattedSize = double( uint64_t(uint64_t(iBytesSize/factor)*digitsPrecision)/tbPrecision )/double(digitsPrecision);
        break;
      };

      //Add formattedSize to friendlySize
      static const int BUFFER_SIZE = 1024;
      char buffer[BUFFER_SIZE];
      sprintf(buffer, "%.2f", formattedSize);
      friendlySize = buffer;

      //Append unit descrition to friendlySize
      switch(iUnit)
      {
      case Bytes:
        {
          friendlySize = strings::toString(iBytesSize);
          friendlySize += " bytes";
        };
        break;
      case Kilobytes:
        {
          friendlySize += " KB";
        };
        break;
      case Megabytes:
        {
          friendlySize += " MB";
        };
        break;
      case Gigabytes:
        {
          friendlySize += " GB";
        };
        break;
      case Terabytes:
        {
          friendlySize += " TB";
        };
        break;
      };

      return friendlySize;
    }

    uint64_t getFileModifiedDate(const std::string & iPath)
    {
      struct stat result;
      uint64_t mod_time = 0;
      if(stat(iPath.c_str(), &result)==0)
      {
        mod_time = result.st_mtime;
      }
      return mod_time;
    }

  } //namespace filesystem
} //namespace ra
