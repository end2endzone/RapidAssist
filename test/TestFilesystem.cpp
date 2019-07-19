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
#include "rapidassist/process.h"
#include "rapidassist/random.h"

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
  TEST_F(TestFilesystem, testGetFilenameWithoutExtension)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string filename = filesystem::getFilenameWithoutExtension(NULL);
      ASSERT_EQ(EXPECTED, filename);
    }

    //test filename only
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::getFilenameWithoutExtension("foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (unix style)
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::getFilenameWithoutExtension("/home/myFolder/foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (windows style)
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::getFilenameWithoutExtension("C:\\Users\\Antoine\\Desktop\\myFolder\\foo.bar");
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
      const std::string pattern = environment::getEnvironmentVariable("windir"); //returns C:\Windows or C:\windows
#else
      const std::string pattern = "/var";
#endif
      //find the exact pattern in the list
      bool found = false;
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & file = files[i];
        found |= (file == pattern);

#ifdef _WIN32
        //fix for multiple Windows system (including AppVeyor) which has 'windir' defined as 'C:\windows' instead of 'C:\Windows'.
        if (pattern == "C:\\windows")
        {
          found |= (file == "C:\\Windows");
        }
#endif
      }
      ASSERT_TRUE(found) << "Failed finding the pattern '" << pattern.c_str() << "' in folder '" << path << "'.\n"
        "Found the following elements: \n" << 
        strings::join(files, "\n").c_str();
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFindFileFromPaths)
  {
    //test no result
    {
      ra::strings::StringVector locations;
      bool success = ra::filesystem::findFileFromPaths("a file that cannot be found", locations);
      ASSERT_FALSE(success);
    }

    //test single result
    {
#ifdef _WIN32
      const std::string filename = "shell32.dll";
#else
      const std::string filename = "man";
#endif
      ra::strings::StringVector locations;
      bool success = ra::filesystem::findFileFromPaths(filename, locations);
      ASSERT_TRUE(success);
      ASSERT_EQ(1, locations.size());
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

    //test issue #27
    {
      std::string temp_dir = ra::filesystem::getTemporaryFolder();

      //build path with subfolders
      const char * separator = getPathSeparatorStr();
      std::string path = temp_dir + separator + ra::gtesthelp::getTestQualifiedName() + "." + ra::strings::toString(__LINE__);
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
  TEST_F(TestFilesystem, testGetTemporaryFolder)
  {
    std::string temp_dir = filesystem::getTemporaryFolder();
    ASSERT_TRUE( !temp_dir.empty() );
    ASSERT_TRUE( ra::filesystem::folderExists(temp_dir.c_str()) );
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
  TEST_F(TestFilesystem, testMakeRelativePath)
  {
    //test deeper directory
    {
#ifdef _WIN32
      const std::string base_path = "C:\\Program Files\\Foo";
      const std::string test_path = "C:\\Program Files\\Foo\\Bar\\Baz";
      const std::string expected_path = "Bar\\Baz";
#else
      const std::string base_path = "/media/cdrom";
      const std::string test_path = "/media/cdrom/Bar/Baz";
      const std::string expected_path = "Bar/Baz";
#endif
      std::string relative_path = ra::filesystem::makeRelativePath(base_path, test_path);
      ASSERT_EQ( expected_path, relative_path );
    }

    //test root directory is the common ground
    {
#ifdef _WIN32
      const std::string base_path = "C:\\Program Files\\GIMP 2";
      const std::string test_path = "C:\\temp\\images";
      const std::string expected_path = "..\\..\\temp\\images";
#else
      const std::string base_path = "/usr/bin";
      const std::string test_path = "/mnt/portable";
      const std::string expected_path = "../../mnt/portable";
#endif
      std::string relative_path = ra::filesystem::makeRelativePath(base_path, test_path);
      ASSERT_EQ( expected_path, relative_path );
    }

    //test first directory is the common ground
    {
#ifdef _WIN32
      const std::string base_path = "C:\\Program Files\\GIMP 2";
      const std::string test_path = "C:\\Program Files\\Microsoft Office";
      const std::string expected_path = "..\\Microsoft Office";
#else
      const std::string base_path = "/usr/bin";
      const std::string test_path = "/usr/sbin";
      const std::string expected_path = "../sbin";
#endif
      std::string relative_path = ra::filesystem::makeRelativePath(base_path, test_path);
      ASSERT_EQ( expected_path, relative_path );
    }

#ifdef _WIN32
    //test unrelated path
    {
      const std::string base_path = "C:\\Program Files\\Microsoft Office";
      const std::string test_path = "D:\\temp";
      std::string relative_path = ra::filesystem::makeRelativePath(base_path, test_path);
      ASSERT_TRUE( relative_path.empty() ) << "relative_path expected to be empty string but is '" << relative_path << "'.";
    }
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
  TEST_F(TestFilesystem, testAbsolutePath)
  {
#ifdef _WIN32
    ASSERT_TRUE(  ra::filesystem::isAbsolutePath("C:\\boot.ini") );
    ASSERT_TRUE(  ra::filesystem::isAbsolutePath("\\\\filesrv1\\public\\list.xml") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath("src\\main.cpp") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath(".\\src\\main.cpp") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath("..\\src\\main.cpp") );
#elif __linux__
    ASSERT_TRUE(  ra::filesystem::isAbsolutePath("/home") );
    ASSERT_TRUE(  ra::filesystem::isAbsolutePath("/bin/bash") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath("src/main.cpp") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath(".//src/main.cpp") );
    ASSERT_FALSE( ra::filesystem::isAbsolutePath("..//src/main.cpp") );
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathBasedOnCurrentProcess)
  {
    //test already absolute
    {
#ifdef _WIN32
      std::string testPath = "C:\\windows\\system32\\cmd.exe";
#elif __linux__
      std::string testPath = "/bin/bash";
#endif
      std::string actual = ra::filesystem::getPathBasedOnCurrentProcess(testPath);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
 
    //test relative path
    {
      std::string testPath = "files\\images\\slashscreen.png";
      ra::filesystem::normalizePath(testPath);

      std::string actual = ra::filesystem::getPathBasedOnCurrentProcess(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
 
    //test filename only
    {
      std::string testPath = "slashscreen.png";
      ra::filesystem::normalizePath(testPath);

      std::string actual = ra::filesystem::getPathBasedOnCurrentProcess(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathBasedOnCurrentDirectory)
  {
    //test already absolute
    {
#ifdef _WIN32
      std::string testPath = "C:\\windows\\system32\\cmd.exe";
#elif __linux__
      std::string testPath = "/bin/bash";
#endif
      std::string actual = ra::filesystem::getPathBasedOnCurrentDirectory(testPath);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
 
    //test relative path
    {
      std::string testPath = "files\\images\\slashscreen.png";
      ra::filesystem::normalizePath(testPath);

      std::string actual = ra::filesystem::getPathBasedOnCurrentDirectory(testPath);
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
 
    //test filename only
    {
      std::string testPath = "slashscreen.png";
      ra::filesystem::normalizePath(testPath);

      std::string actual = ra::filesystem::getPathBasedOnCurrentDirectory(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath01)
  {
    //test with ..
    {
      SCOPED_TRACE("test with ..");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\..\\baz\\myapp.exe";
#elif __linux__
      std::string testPath = "/foo/bar/../baz/myapp";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\baz\\myapp.exe", actual);
#elif __linux__
      ASSERT_EQ("/foo/baz/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath02)
  {
    //test with .. at the end
    {
      SCOPED_TRACE("test with .. at the end");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\..";
#elif __linux__
      std::string testPath = "/foo/bar/..";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );

#ifdef _WIN32
      ASSERT_EQ("C:\\foo", actual);
#elif __linux__
      ASSERT_EQ("/foo", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath03)
  {
    //test with .
    {
      SCOPED_TRACE("test with .");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\.\\baz\\myapp.exe";
#elif __linux__
      std::string testPath = "/foo/bar/./baz/myapp";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("/./")); // . element removed from path
      ASSERT_EQ(std::string::npos, actual.find("\\.\\")); // . element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\bar\\baz\\myapp.exe", actual);
#elif __linux__
      ASSERT_EQ("/foo/bar/baz/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath04)
  {
    //test with . at the end
    {
      SCOPED_TRACE("test with . at the end");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\.";
#elif __linux__
      std::string testPath = "/foo/bar/.";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("/./")); // . element removed from path
      ASSERT_EQ(std::string::npos, actual.find("\\.\\")); // . element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\bar", actual);
#elif __linux__
      ASSERT_EQ("/foo/bar", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath05)
  {
    //test too many \..\ elements
    {
      SCOPED_TRACE("test too many \\..\\ elements");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\..\\..\\..\\myapp.exe";
#elif __linux__
      std::string testPath = "/foo/../../../myapp";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE( ra::filesystem::isAbsolutePath(actual) );

#ifdef _WIN32
      ASSERT_EQ("C:\\myapp.exe", actual);
#elif __linux__
      ASSERT_EQ("/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath06)
  {
    //test with impossible to resolve situations (relative path)
    {
      SCOPED_TRACE("test with impossible to resolve situations (relative path)");
#ifdef _WIN32
      std::string testPath = "..\\foo\\..\\..\\bar\\baz\\..\\myapp.exe";
#elif __linux__
      std::string testPath = "../foo/../../bar/baz/../myapp";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());

#ifdef _WIN32
      ASSERT_EQ("..\\..\\bar\\myapp.exe", actual);
#elif __linux__
      ASSERT_EQ("../../bar/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath07)
  {
    //test buggy path. See issue #11 https://github.com/end2endzone/RapidAssist/issues/11
    {
      SCOPED_TRACE("test buggy path");
#ifdef _WIN32
      std::string testPath = "\\home\\pi\\dev\\github\\RapidAssist\\build\\bin\\files\\images\\slashscreen.png";
#elif __linux__
      std::string testPath = "/home/pi/dev/github/RapidAssist/build/bin/files/images/slashscreen.png";
#endif
      std::string actual = ra::filesystem::resolvePath(testPath);
      SCOPED_TRACE(actual);
 
      ASSERT_FALSE(actual.empty());

#ifdef _WIN32
      ASSERT_EQ(testPath, actual);
#elif __linux__
      ASSERT_EQ(testPath, actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testIsRootDirectory)
  {
#ifdef _WIN32
    ASSERT_TRUE( ra::filesystem::isRootDirectory("C:\\") );
    ASSERT_TRUE( ra::filesystem::isRootDirectory("c:\\") );
    ASSERT_TRUE( ra::filesystem::isRootDirectory("z:\\") );
    ASSERT_FALSE( ra::filesystem::isRootDirectory("c:\\foo") );
    ASSERT_FALSE( ra::filesystem::isRootDirectory("c:") );
#elif __linux__
    ASSERT_TRUE( ra::filesystem::isRootDirectory("/") );
    ASSERT_FALSE( ra::filesystem::isRootDirectory("/foo") );
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testCopyFileBasic)
  {
    //copy this process executable to another file
    const std::string process_path = ra::process::getCurrentProcessPath();
    const std::string process_filename = ra::filesystem::getFilename(process_path.c_str());
    const std::string temp_dir = filesystem::getTemporaryFolder();
    const std::string output_filename = ra::gtesthelp::getTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::getPathSeparator() + output_filename;

    ASSERT_TRUE( ra::filesystem::fileExists(process_path.c_str()) );
    ASSERT_TRUE( ra::filesystem::folderExists(temp_dir.c_str()) );
    
    bool copied = ra::filesystem::copyFile(process_path, output_path);
    ASSERT_TRUE( copied ) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE( ra::filesystem::fileExists(output_path.c_str()) ) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::getFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::getFileSize(output_path.c_str());
    ASSERT_EQ( source_size, target_size );
  }
  //--------------------------------------------------------------------------------------------------
  static bool gProgressBegin = false;
  static bool gProgressEnd   = false;
  void myCopyFileCallbackFunction(double progress)
  {
    //remember first and last callbacks
    if (progress == 0.0)
      gProgressBegin = true;
    if (progress == 1.0)
      gProgressEnd = true;

    printf("%s(%.2f)\n", __FUNCTION__, progress);
  }
  TEST_F(TestFilesystem, testCopyFileCallbackFunction)
  {
    //copy this process executable to another file
    const std::string process_path = ra::process::getCurrentProcessPath();
    const std::string process_filename = ra::filesystem::getFilename(process_path.c_str());
    const std::string temp_dir = filesystem::getTemporaryFolder();
    const std::string output_filename = ra::gtesthelp::getTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::getPathSeparator() + output_filename;

    ASSERT_TRUE( ra::filesystem::fileExists(process_path.c_str()) );
    ASSERT_TRUE( ra::filesystem::folderExists(temp_dir.c_str()) );
    
    gProgressBegin = false;
    gProgressEnd   = false;
    bool copied = ra::filesystem::copyFile(process_path, output_path, &myCopyFileCallbackFunction);
    ASSERT_TRUE( copied ) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE( ra::filesystem::fileExists(output_path.c_str()) ) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::getFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::getFileSize(output_path.c_str());
    ASSERT_EQ( source_size, target_size );

    //assert that first and last progress was received
    ASSERT_TRUE(gProgressBegin);
    ASSERT_TRUE(gProgressEnd  );
  }
  //--------------------------------------------------------------------------------------------------
  class CopyFileCallbackFunctor : public virtual ra::filesystem::IProgressReport
  {
  public:
    CopyFileCallbackFunctor() : mProgressBegin(false), mProgressEnd(false) {};
    virtual ~CopyFileCallbackFunctor() {};
    virtual void onProgressReport(double progress)
    {
      //remember first and last callbacks
      if (progress == 0.0)
        mProgressBegin = true;
      if (progress == 1.0)
        mProgressEnd = true;

      printf("%s(%.2f)\n", __FUNCTION__, progress);
    }
    bool hasProgressBegin() { return mProgressBegin; }
    bool hasProgressEnd  () { return mProgressEnd  ; }
  private:
    bool mProgressBegin;
    bool mProgressEnd  ;
  };
  TEST_F(TestFilesystem, testCopyFileCallbackFunctor)
  {
    //copy this process executable to another file
    const std::string process_path = ra::process::getCurrentProcessPath();
    const std::string process_filename = ra::filesystem::getFilename(process_path.c_str());
    const std::string temp_dir = filesystem::getTemporaryFolder();
    const std::string output_filename = ra::gtesthelp::getTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::getPathSeparator() + output_filename;

    ASSERT_TRUE( ra::filesystem::fileExists(process_path.c_str()) );
    ASSERT_TRUE( ra::filesystem::folderExists(temp_dir.c_str()) );
    
    CopyFileCallbackFunctor functor;
    bool copied = ra::filesystem::copyFile(process_path, output_path, &functor);
    ASSERT_TRUE( copied ) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE( ra::filesystem::fileExists(output_path.c_str()) ) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::getFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::getFileSize(output_path.c_str());
    ASSERT_EQ( source_size, target_size );

    //assert that first and last progress was received
    ASSERT_TRUE(functor.hasProgressBegin());
    ASSERT_TRUE(functor.hasProgressEnd  ());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadFile)
  {
    //test file not found
    {
      const std::string file_path = "this file is not found";
      std::string content;
      bool success = ra::filesystem::readFile(file_path, content);
      ASSERT_FALSE( success );
    }

    //test file read
    {
      const std::string file_path = ra::process::getCurrentProcessPath();
      std::string content;
      bool success = ra::filesystem::readFile(file_path, content);
      ASSERT_TRUE( success );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteFile)
  {
    //test write fail
    {
      const std::string file_path = ra::filesystem::getCurrentFolder() +
                                    ra::filesystem::getPathSeparatorStr() + "foo" +
                                    ra::filesystem::getPathSeparatorStr() + "bar" +
                                    ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".bin"; //folder 'foo\bar' does not exists.
      const size_t content_size = (size_t)ra::random::getRandomInt(1300, 13000);
      const std::string content = ra::random::getRandomString(content_size);
      bool success = ra::filesystem::writeFile(file_path, content);
      ASSERT_FALSE( success );
    }

    //test random content write
    {
      const std::string file_path = ra::gtesthelp::getTestQualifiedName() + ".bin";
      const size_t content_size = (size_t)ra::random::getRandomInt(1300, 13000);
      const std::string content = ra::random::getRandomString(content_size);
      bool success = ra::filesystem::writeFile(file_path, content);
      ASSERT_TRUE( success );

      //assert write success
      const bool found = ra::filesystem::fileExists(file_path.c_str());
      const size_t write_size = ra::filesystem::getFileSize(file_path.c_str());
      ASSERT_EQ( content_size, write_size );

      //cleanup
      ra::filesystem::deleteFile(file_path.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadWriteFileDataLoss)
  {
    const std::string file_path = ra::gtesthelp::getTestQualifiedName() + ".bin";
    const size_t content_size = (size_t)ra::random::getRandomInt(1300, 13000);
    const std::string content_write = ra::random::getRandomString(content_size);
    bool success = ra::filesystem::writeFile(file_path, content_write);
    ASSERT_TRUE( success );

    //assert write success
    const size_t write_size = ra::filesystem::getFileSize(file_path.c_str());
    ASSERT_EQ( content_size, write_size );

    std::string content_read;
    success = ra::filesystem::readFile(file_path, content_read);
    ASSERT_TRUE( success );

    //assert that we readed the same amount of data
    ASSERT_EQ( content_write.size(), content_read.size() );
    ASSERT_EQ( content_write, content_read ); //and the same data

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadTextFile)
  {
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
      "The"   + newline +
      "quick" + newline +
      "brown" + newline +
      "fox"   + newline +
      "jumps" + newline +
      "over"  + newline +
      "the"   + newline +
      "lazy"  + newline +
      "dog." ;
    const std::string file_path = ra::gtesthelp::getTestQualifiedName() + ".txt";
    bool success = ra::filesystem::writeFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //read as lines
    ra::strings::StringVector lines;
    bool readok = readTextFile(file_path, lines, true);
    ASSERT_TRUE( readok );

    //assert each lines properly readed
    static const size_t num_expected_lines = 9;
    ASSERT_EQ(num_expected_lines, lines.size());
    ASSERT_EQ(std::string("The"  ), lines[0]);
    ASSERT_EQ(std::string("quick"), lines[1]);
    ASSERT_EQ(std::string("brown"), lines[2]);
    ASSERT_EQ(std::string("fox"  ), lines[3]);
    ASSERT_EQ(std::string("jumps"), lines[4]);
    ASSERT_EQ(std::string("over" ), lines[5]);
    ASSERT_EQ(std::string("the"  ), lines[6]);
    ASSERT_EQ(std::string("lazy" ), lines[7]);
    ASSERT_EQ(std::string("dog." ), lines[8]);

    //read as a single buffer
    std::string buffer;
    readok = readTextFile(file_path, buffer);
    ASSERT_TRUE( readok );

    //assert content is properly readed
#ifdef _WIN32
    // CRLF characters are replaced by CR when a file is readed in text mode
    std::string expected = content;
    ra::strings::replace(expected, newline, "\n");
#else
    std::string expected = content;
#endif
    ASSERT_EQ( expected.size(), buffer.size() );
    ASSERT_EQ( expected, buffer );

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteTextFileBuffer)
  {
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
      "The"   + newline +
      "quick" + newline +
      "brown" + newline +
      "fox"   + newline +
      "jumps" + newline +
      "over"  + newline +
      "the"   + newline +
      "lazy"  + newline +
      "dog." ;
    const std::string file_path = ra::gtesthelp::getTestQualifiedName() + ".txt";
    bool success = ra::filesystem::writeFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //assert file size
#ifdef _WIN32
    const size_t expected_file_size = 52;
#else
    const size_t expected_file_size = 52-8; // 1 byte per newline less than windows version
#endif
    const size_t file_size = ra::filesystem::getFileSize(file_path.c_str());
    ASSERT_EQ( expected_file_size, file_size );

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteTextFileLines)
  {
    const std::string newline = ra::environment::getLineSeparator();

    //building word list
    static const std::string sentence = "The quick brown fox jumps over the lazy dog.";
    ra::strings::StringVector word_list;
    ra::strings::split(word_list, sentence, " ");

    //assert writing without adding new line between words
    const std::string file_path1 = ra::gtesthelp::getTestQualifiedName() + ".1.txt";
    bool success = ra::filesystem::writeTextFile(file_path1, word_list, false);
    ASSERT_TRUE( success );

    //read the generated file as a binary string
    std::string binary;
    bool readed = ra::filesystem::readFile(file_path1, binary);
    ASSERT_TRUE( readed );

    //assert no newline between words
    static const std::string expected_binary = "Thequickbrownfoxjumpsoverthelazydog.";
    ASSERT_EQ( expected_binary, binary );

    //assert file size
    {
      const size_t expected_file_size = 36;
      const size_t file_size = ra::filesystem::getFileSize(file_path1.c_str());
      ASSERT_EQ( expected_file_size, file_size );
    }

    //assert writing with new line between words
    const std::string file_path2 = ra::gtesthelp::getTestQualifiedName() + ".2.txt";
    success = ra::filesystem::writeTextFile(file_path2, word_list, true);
    ASSERT_TRUE( success );

    //read the generated file as a binary string
    readed = ra::filesystem::readFile(file_path2, binary);
    ASSERT_TRUE( readed );

    //assert newline between words
    {
      const size_t minimum_file_size = 40;
      const size_t file_size = ra::filesystem::getFileSize(file_path2.c_str());
      ASSERT_GT( file_size, minimum_file_size );
    }

    //cleanup
    ra::filesystem::deleteFile(file_path1.c_str());
    ra::filesystem::deleteFile(file_path2.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace filesystem
} //namespace ra
