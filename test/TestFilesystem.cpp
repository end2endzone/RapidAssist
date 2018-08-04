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

#include "TestFilesystem.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/time_.h"
#include "rapidassist/gtesthelp.h"
#include "rapidassist/environment.h"

#ifndef _WIN32
#include <linux/fs.h>
#include <sys/ioctl.h> //for ioctl()
#endif

namespace ra { namespace filesystem { namespace test
{
  int countValues(const std::vector<std::string> & iList, const std::string & iValue)
  {
    int count = 0;
    for(size_t i=0; i<iList.size(); i++)
    {
      const std::string & value = iList[i];
      if (value == iValue)
        count++;
    }
    return count;
  }

  bool createCarsDirectory(const std::string & iBasePath)
  {
    // iBasePath
    // |-cars
    //   |-prices.txt
    //   |-Mazda (empty folder)
    //   |
    //   |-Honda
    //   | |-Civic.txt
    //   |
    //   |-Toyota
    //   | |-Corolla.txt
    //   | |-Camry.txt
    //   |
    //   |-Volkswagen
    //     |-Passat.txt
    //     |-Golf.txt
    //     |-Jetta.txt

    //create folders
    ra::strings::StringVector folders;
    folders.push_back(iBasePath + "/cars/Mazda");
    folders.push_back(iBasePath + "/cars/Honda");
    folders.push_back(iBasePath + "/cars/Toyota");
    folders.push_back(iBasePath + "/cars/Volkswagen");
    for(size_t i=0; i<folders.size(); i++)
    {
      std::string & folder = folders[i];
      filesystem::normalizePath(folder);

      bool success = filesystem::createFolder(folder.c_str());
      if (!success)
        return false;
    }

    //create the files
    ra::strings::StringVector files;
    files.push_back(iBasePath + "/cars/prices.txt");
    files.push_back(iBasePath + "/cars/Honda/Civic.txt");
    files.push_back(iBasePath + "/cars/Toyota/Corolla.txt");
    files.push_back(iBasePath + "/cars/Toyota/Camry.txt");
    files.push_back(iBasePath + "/cars/Volkswagen/Passat.txt");
    files.push_back(iBasePath + "/cars/Volkswagen/Golf.txt");
    files.push_back(iBasePath + "/cars/Volkswagen/Jetta.txt");
    for(size_t i=0; i<files.size(); i++)
    {
      std::string & file = files[i];
      filesystem::normalizePath(file);

      bool success = ra::gtesthelp::createFile( file.c_str() );
      if (!success)
        return false;
    }

    return true;
  }

  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testNormalizePath)
  {
#ifdef _WIN32
    //test properly formatted path
    {
      static const std::string EXPECTED = "C:\\temp\\foo\\bar.txt";
      std::string path = EXPECTED;
      filesystem::normalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }

    //test incorrectly formatted path
    {
      static const std::string EXPECTED = "C:\\temp\\foo\\bar.txt";
      std::string path = "C:/temp/foo/bar.txt";
      filesystem::normalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }
#else
    //test properly formatted path
    {
      static const std::string EXPECTED = "/tmp/foo/bar.txt";
      std::string path = EXPECTED;
      filesystem::normalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }

    //test incorrectly formatted path
    {
      static const std::string EXPECTED = "/tmp/foo/bar.txt";
      std::string path = "\\tmp\\foo\\bar.txt";
      filesystem::normalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileSize)
  {
    //test NULL
    {
      const char * path = NULL;
      uint32_t size = filesystem::getFileSize(path);
      ASSERT_EQ(0, size);
    }

    //test actual value
    {
      //create dummy file
      std::string filename = ra::gtesthelp::getTestQualifiedName();
      ASSERT_TRUE( ra::gtesthelp::createFile(filename.c_str()) );

#ifdef WIN32
      static const uint32_t EXPECTED = 14;
#elif __linux__
      static const uint32_t EXPECTED = 11;
#endif

      //test `const char *` api
      uint32_t size = filesystem::getFileSize(filename.c_str());
      ASSERT_EQ(EXPECTED, size);
      size = 0;

      //test `FILE*` api
      FILE * ptr = fopen(filename.c_str(), "r");
      ASSERT_TRUE(ptr != NULL);
      size = filesystem::getFileSize(ptr);
      fclose(ptr);
      ASSERT_EQ(EXPECTED, size);
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFilename)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string filename = filesystem::getFilename(NULL);
      ASSERT_EQ(EXPECTED, filename);
    }

    //test filename only
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (unix style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("/home/myFolder/foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (windows style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("C:\\Users\\Antoine\\Desktop\\myFolder\\foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFileExists)
  {
    //test NULL
    {
      bool exists = filesystem::fileExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::fileExists("foo.bar.notfound.bang");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //create dummy file
      std::string filename = ra::gtesthelp::getTestQualifiedName();
      ASSERT_TRUE( ra::gtesthelp::createFile(filename.c_str()) );

      bool exists = filesystem::fileExists(filename.c_str());
      ASSERT_TRUE(exists);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFindFiles)
  {
    //test NULL
    {
      ra::strings::StringVector files;
      bool success = filesystem::findFiles(files, NULL);
      ASSERT_FALSE(success);
    }

    //test current directory
    {
      ra::strings::StringVector files;
      bool success = filesystem::findFiles(files, ".", -1);
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0 );
    }

    //create cars directory tree
    std::string basePath = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
    {
      bool carsOK = createCarsDirectory(basePath);
      ASSERT_TRUE(carsOK);
    }

    //test subdirectory
    {
      ra::strings::StringVector files;
      bool success = filesystem::findFiles(files, basePath.c_str());
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0 );

      //search for known values
      bool hasMazdaFolder = false; //test finding empty folders
      bool hasHondaFolder = false; //test non-empty folder
      bool hasPricesFile = false; //test for non-leaf files
      bool hasJettaFile = false; //test last filename
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & entry = files[i];
        if (entry.find("Mazda") != std::string::npos)
        {
          hasMazdaFolder = true;
        }
        else if (entry.find("Honda") != std::string::npos)
        {
          hasHondaFolder = true;
        }
        else if (entry.find("prices.txt") != std::string::npos)
        {
          hasPricesFile = true;
        }
        else if (entry.find("Jetta") != std::string::npos)
        {
          hasJettaFile = true;
        }
      }
      ASSERT_TRUE(hasMazdaFolder);
      ASSERT_TRUE(hasHondaFolder);
      ASSERT_TRUE(hasPricesFile);
      ASSERT_TRUE(hasJettaFile);
    }

    //test depth
    {
      ra::strings::StringVector files;
      bool success = filesystem::findFiles(files, basePath.c_str(), 1); //cars folders is found at level 1, cars direct subfolder and files are found at level 0.
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0 );

      //search for known values
      bool hasMazdaFolder = false; //test finding empty folders
      bool hasHondaFolder = false; //test non-empty folder
      bool hasPricesFile = false; //test for non-leaf files, leafs at level 1
      bool hasJettaFile = false; //test for filenames at depth 2
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & entry = files[i];
        if (entry.find("Mazda") != std::string::npos)
        {
          hasMazdaFolder = true;
        }
        else if (entry.find("Honda") != std::string::npos)
        {
          hasHondaFolder = true;
        }
        else if (entry.find("prices.txt") != std::string::npos)
        {
          hasPricesFile = true;
        }
        else if (entry.find("Jetta") != std::string::npos)
        {
          hasJettaFile = true;
        }
      }
      ASSERT_TRUE (hasMazdaFolder);
      ASSERT_TRUE (hasHondaFolder);
      ASSERT_TRUE(hasPricesFile);
      ASSERT_FALSE(hasJettaFile);
    }

    //test root file system
    {
#ifdef _WIN32
      const char * path = "C:\\";
#else
      const char * path = "/";
#endif
      ra::strings::StringVector files;
      bool success = filesystem::findFiles(files, path, 1);
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0 );

      //assert normalization
#ifdef _WIN32
      ASSERT_EQ( files[0].find("C:\\\\"), std::string::npos ); //assert that C:\\ (double backslashes) is not found
#else
      ASSERT_EQ( files[0].find("//"), std::string::npos ); //assert that // (double slashes) is not found
#endif

      //search for the last (almost) folder of the root file system.
#ifdef _WIN32
      //fix for appveyor
      std::string windir = environment::getEnvironmentVariable("windir"); //returns C:\Windows
      if (ra::gtesthelp::isAppVeyor() && windir == "C:\\windows")
      {
        //fix for appveyor which has 'windir' defined as 'C:\windows' instead of 'C:\Windows'.
        windir = "C:\\Windows";
      }

      const std::string pattern = windir;
#else
      const std::string pattern = "/var";
#endif
      //find the exact pattern in the list
      bool found = false;
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & file = files[i];
        found |= (file == pattern);
      }
      ASSERT_TRUE(found) << "Failed finding the pattern '" << pattern.c_str() << "' in folder '" << path << "'.\n"
        "Found the following elements: \n" << 
        strings::joinString(files, "\n").c_str();
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFolderExists)
  {
    //test NULL
    {
      bool exists = filesystem::folderExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::folderExists("/home/fooBAR");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //get dummy folder
      std::string currentFolder = filesystem::getCurrentFolder();
      ASSERT_TRUE( !currentFolder.empty() );

      bool exists = filesystem::folderExists(currentFolder.c_str());
      ASSERT_TRUE(exists);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testCreateFolder)
  {
    //test NULL
    {
      bool success = filesystem::createFolder(NULL);
      ASSERT_FALSE(success);
    }

    //test current folder
    {
      bool success = filesystem::createFolder("."); //should return true as the folder already exists
      ASSERT_TRUE(success);
    }

    //test folder already exits (call twice)
    {
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
      bool success = false;
      success = filesystem::createFolder(path.c_str());
      ASSERT_TRUE(success);

      //call createFolder() twice should still be a success
      success = filesystem::createFolder(path.c_str());
      ASSERT_TRUE(success);

      //cleanup
      deleteFolder(path.c_str());
    }

    //test subfolders
    {
      //build path with subfolders
      char separator[] = {getPathSeparator(), '\0'};
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
      path << separator << "1" << separator << "2" << separator << "3" << separator << "4" << separator << "5";

      bool success = false;
      success = filesystem::createFolder(path.c_str());
      ASSERT_TRUE(success);

      //cleanup
      deleteFolder(path.c_str());
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testDeleteFile)
  {
    //test NULL
    {
      bool success = filesystem::deleteFile(NULL);
      ASSERT_FALSE(success);
    }

    //test success
    {
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__) + ".txt";
      bool success = ra::gtesthelp::createFile(path.c_str());
      ASSERT_TRUE(success);

      success = filesystem::deleteFile(path.c_str());
      ASSERT_TRUE(success);

      //assert file is actually deleted
      bool found = filesystem::fileExists(path.c_str());
      ASSERT_FALSE(found);
    }
 
    //test failure
    {
#ifndef _WIN32
      return; //test failure unsupported. See below.
#endif
      
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__) + ".txt";
      bool success = ra::gtesthelp::createFile(path.c_str());
      ASSERT_TRUE(success);

      //open the file so it cannot be deleted
      FILE * f = fopen(path.c_str(), "w+");
      ASSERT_TRUE(f != NULL);

#ifndef _WIN32
      //On linux, an open file can be deleted.
      //the 'immutable' flag can be set to prevents the file to be deleted
      //but this requires sudo privileges
      //int flags = FS_IMMUTABLE_FL;
      //if(ioctl(fileno(f), FS_IOC_SETFLAGS, &flags) < 0) //set the immutable flag
      //  perror("ioctl error");
#endif

      success = filesystem::deleteFile(path.c_str());
      ASSERT_FALSE(success);

#ifndef _WIN32
      //unset the 'immutable' flag allow the file to be deleted.
      //flags = 0;
      //ioctl(fileno(f), FS_IOC_SETFLAGS, &flags); //unset the immutable flag
#endif

      //release the file
      fclose(f);

      //try to delete again
      success = filesystem::deleteFile(path.c_str());
      ASSERT_TRUE(success);

      //assert the file is really deleted
      bool found = filesystem::fileExists(path.c_str());
      ASSERT_FALSE(found);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testDeleteFolder)
  {
    //test NULL
    {
      bool success = filesystem::deleteFolder(NULL);
      ASSERT_FALSE(success);
    }

    //create cars directory tree
    std::string basePath = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
    {
      bool carsOK = createCarsDirectory(basePath);
      ASSERT_TRUE(carsOK);
    }

    //test success
    {
      bool success = filesystem::deleteFolder(basePath.c_str());
      ASSERT_TRUE(success);

      //assert folder is actually deleted
      ASSERT_FALSE( filesystem::folderExists(basePath.c_str()) );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFileName)
  {
    //test not empty
    {
      std::string filename = filesystem::getTemporaryFileName();
      ASSERT_TRUE( !filename.empty() );
    }

    //test repetitive
    {
      std::vector<std::string> filenames;
      static const size_t numTest = 20;
      for(size_t i=0; i<numTest; i++)
      {
        std::string filename = filesystem::getTemporaryFileName();
        filenames.push_back(filename);
      }

      //assert that all values are unique
      for(size_t i=0; i<numTest; i++)
      {
        const std::string & filename = filenames[i];
        int count = countValues(filenames, filename);
        ASSERT_EQ(1, count) << "Found value '" << filename << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFilePath)
  {
    //test not empty
    {
      std::string path = filesystem::getTemporaryFilePath();
      ASSERT_TRUE( !path.empty() );
    }

    //test repetitive
    {
      std::vector<std::string> paths;
      static const size_t numTest = 20;
      for(size_t i=0; i<numTest; i++)
      {
        std::string path = filesystem::getTemporaryFilePath();
        paths.push_back(path);
      }

      //assert that all values are unique
      for(size_t i=0; i<numTest; i++)
      {
        const std::string & path = paths[i];
        int count = countValues(paths, path);
        ASSERT_EQ(1, count) << "Found value '" << path << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetParentPath)
  {
    //test no folder
    {
      static const std::string EXPECTED = "";
      std::string parent = filesystem::getParentPath("filename.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test unix style
    {
      static const std::string EXPECTED = "/home/myFolder";
      std::string parent = filesystem::getParentPath("/home/myFolder/foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test windows style
    {
      static const std::string EXPECTED = "C:\\Users\\Antoine\\Desktop\\myFolder";
      std::string parent = filesystem::getParentPath("C:\\Users\\Antoine\\Desktop\\myFolder\\foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetShortPathForm)
  {
    //test spaces in filename
    {
      static const std::string EXPECTED = "ABC~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("a b c.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long file extension
    {
      static const std::string EXPECTED = "ABCDEF~1.TEX";
      std::string shortPath = filesystem::getShortPathForm("abcdefgh.text");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long filename
    {
      static const std::string EXPECTED = "ABCDEF~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("abcdefghijklmnopqrstuvwxyz.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test spaces in file extension
    {
      static const std::string EXPECTED = "ABCDE~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("abcde.t x t");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test program files (windows style)
    {
      static const std::string EXPECTED = "PROGRA~1";
      std::string shortPath = filesystem::getShortPathForm("Program Files (x86)");
      ASSERT_EQ(EXPECTED, shortPath);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPath)
  {
    //test baseline
    {
      static const std::string EXPECTED_PARENT = "/home/myFolder";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::splitPath("/home/myFolder/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test empty
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "";
      std::string parent, filename;
      filesystem::splitPath("", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test single filename
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "myfile.txt";
      std::string parent, filename;
      filesystem::splitPath("myfile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test spaces in folder
    {
      static const std::string EXPECTED_PARENT = "/home/my Folder";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::splitPath("/home/my Folder/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test filename without extension / folder name
    {
      static const std::string EXPECTED_PARENT = "/home/myFolder";
      static const std::string EXPECTED_FILENAME = "myFile";
      std::string parent, filename;
      filesystem::splitPath("/home/myFolder/myFile", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    std::string folder;
    std::string filename;
  
    //from full path
    {
      splitPath("C:\\foo\\bar\\file.txt", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      splitPath("C:\\foo\\bar\\file", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file");
    }
    {
      splitPath("C:\\foo\\bar\\file.", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.");
    }

    //from filename only
    {
      splitPath("file.txt", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      splitPath("file.", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file.");
    }
    {
      splitPath("file", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file");
    }

    //empty strings
    {
      splitPath("", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "");
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPathArray)
  {
    //test baseline
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/myFolder/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myFolder", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test empty
    {
      std::vector<std::string> elements;
      filesystem::splitPath("", elements);
      ASSERT_EQ(0, elements.size());
    }

    //test single filename
    {
      std::vector<std::string> elements;
      filesystem::splitPath("myfile.txt", elements);
      ASSERT_EQ(1, elements.size());
      ASSERT_EQ("myfile.txt", elements[0]);
    }

    //test spaces in folder
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/my Folder/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("my Folder", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test filename without extension / folder name
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/myFolder/myFile", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myFolder", elements[1]);
      ASSERT_EQ("myFile", elements[2]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathSeparator)
  {
#ifdef WIN32
      ASSERT_EQ('\\', filesystem::getPathSeparator());
#elif __linux__
      ASSERT_EQ('/', filesystem::getPathSeparator());
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetCurrentFolder)
  {
    std::string curdir = getCurrentFolder();
    ASSERT_NE("", curdir);

    ASSERT_TRUE(filesystem::folderExists(curdir.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileExtention)
  {
    //test baseline
    {
      static const std::string EXPECTED = "txt";
      std::string ext = filesystem::getFileExtention("myFile.txt");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test empty
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test spaces in extension
    {
      static const std::string EXPECTED = "foo bar";
      std::string ext = filesystem::getFileExtention("/home/my Folder/myFile.foo bar");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test filename without extension / folder name
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("/home/myFolder/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test folder with an extension and file without extension
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("/home/my.Folder/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //from valid filename
    {
      std::string ext = getFileExtention("file.txt");
      ASSERT_EQ(ext, "txt");
    }
    {
      std::string ext = getFileExtention("file.");
      ASSERT_EQ(ext, "");
    }
    {
      std::string ext = getFileExtention("file");
      ASSERT_EQ(ext, "");
    }

    //from full path
    {
      std::string ext = getFileExtention("C:\\foo\\bar\\file.txt");
      ASSERT_EQ(ext, "txt");
    }

    //from filename with multiple dots
    {
      std::string ext = getFileExtention("file.subfile.txt");
      ASSERT_EQ(ext, "txt");
    }

    //empty strings
    {
      std::string ext = getFileExtention("");
      ASSERT_EQ(ext, "");
    }

    //from special case path
    {
      std::string ext = getFileExtention("C:\\foo.bar\\file");
      ASSERT_EQ(ext, "");
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetUserFriendlySize)
  {
    static const uint64_t MULTIPLICATOR_KB = 1024;
    static const uint64_t MULTIPLICATOR_MB = 1024*MULTIPLICATOR_KB;
    static const uint64_t MULTIPLICATOR_GB = 1024*MULTIPLICATOR_MB;
    static const uint64_t MULTIPLICATOR_TB = 1024*MULTIPLICATOR_GB;

    {
      static const std::string EXPECTED = "0 bytes";
      std::string size = filesystem::getUserFriendlySize(0ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1023 bytes";
      std::string size = filesystem::getUserFriendlySize(1023ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 KB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 MB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 MB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 GB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 GB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 TB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 TB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_TB);
      ASSERT_EQ(EXPECTED, size);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileModifiedDate)
  {
    //assert that unit of return value is seconds
    {
      //synchronize to the beginning of a new second on wall-clock.
      ra::time::waitNextSecond();

      static const uint64_t EXPECTED = 3;
      const std::string filename1 = ra::gtesthelp::getTestQualifiedName() + ".1.txt";
      const std::string filename2 = ra::gtesthelp::getTestQualifiedName() + ".2.txt";
      ASSERT_TRUE( ra::gtesthelp::createFile(filename1.c_str()) );
      //allow 3 seconds between the files
      for(uint64_t i=0; i<EXPECTED; i++)
      {
        ra::time::waitNextSecond();
      }
      ASSERT_TRUE( ra::gtesthelp::createFile(filename2.c_str()) );

      uint64_t time1 = filesystem::getFileModifiedDate(filename1);
      uint64_t time2 = filesystem::getFileModifiedDate(filename2);
      uint64_t diff = time2 - time1;
      ASSERT_GE(diff, EXPECTED);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasReadAccess)
  {
    //test NULL
    {
      bool result = filesystem::hasReadAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test read access
    {
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
      ASSERT_TRUE( ra::gtesthelp::createFile(path.c_str()) );

      bool hasRead = filesystem::hasReadAccess(path.c_str());
      ASSERT_TRUE(hasRead);

      //cleanup
      filesystem::deleteFile(path.c_str());
    }

    //test no read access
    {
#ifdef _WIN32
      //not supported. Cannot find a file that exists but cannot be read.
      //Note, the file 'C:\pagefile.sys' can be found using FindFirstFile() but not with _stat() which I don't understand.
      return;
#else
      const char * path = "/proc/sysrq-trigger"; //permission denied file
      ASSERT_TRUE( filesystem::fileExists(path) );
      bool hasRead = filesystem::hasReadAccess(path);
      ASSERT_FALSE(hasRead);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasWriteAccess)
  {
    //test NULL
    {
      bool result = filesystem::hasWriteAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test write access
    {
      std::string path = ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
      ASSERT_TRUE( ra::gtesthelp::createFile(path.c_str()) );

      bool hasWrite = filesystem::hasWriteAccess(path.c_str());
      ASSERT_TRUE(hasWrite);

      //cleanup
      filesystem::deleteFile(path.c_str());
    }

    //test no write access
    {
#ifdef _WIN32
      const char * path = "C:\\bootmgr"; //premission denied file. Could also use `C:\Users\All Users\ntuser.pol'`
#else
      const char * path = "/proc/cpuinfo"; //permission denied file
#endif
      ASSERT_TRUE( filesystem::fileExists(path) );
      bool hasWrite = filesystem::hasWriteAccess(path);
      ASSERT_FALSE(hasWrite);
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace filesystem
} //namespace ra
