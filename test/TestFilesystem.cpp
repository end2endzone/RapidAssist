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
#include "rapidassist/timing.h"
#include "rapidassist/testing.h"
#include "rapidassist/environment.h"
#include "rapidassist/process.h"
#include "rapidassist/random.h"

#ifdef __linux__
#include <linux/fs.h>
#endif

#ifndef _WIN32
#include <sys/ioctl.h> //for ioctl()
#endif

namespace ra { namespace filesystem { namespace test
{

  int CountValues(const std::vector<std::string> & list, const std::string & value) {
    int count = 0;
    for (size_t i = 0; i < list.size(); i++) {
      const std::string & tmp = list[i];
      if (tmp == value)
        count++;
    }
    return count;
  }

  bool CreateCarsDirectory(const std::string & base_path) {
    // base_path
    // |-cars
    //   |-prices.txt
    //   |-Mazda (empty directory)
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

    //create directories
    ra::strings::StringVector directories;
    directories.push_back(base_path + "/cars/Mazda");
    directories.push_back(base_path + "/cars/Honda");
    directories.push_back(base_path + "/cars/Toyota");
    directories.push_back(base_path + "/cars/Volkswagen");
    for (size_t i = 0; i < directories.size(); i++) {
      std::string & directory = directories[i];
      filesystem::NormalizePath(directory);

      bool success = filesystem::CreateDirectory(directory.c_str());
      if (!success)
        return false;
    }

    //create the files
    ra::strings::StringVector files;
    files.push_back(base_path + "/cars/prices.txt");
    files.push_back(base_path + "/cars/Honda/Civic.txt");
    files.push_back(base_path + "/cars/Toyota/Corolla.txt");
    files.push_back(base_path + "/cars/Toyota/Camry.txt");
    files.push_back(base_path + "/cars/Volkswagen/Passat.txt");
    files.push_back(base_path + "/cars/Volkswagen/Golf.txt");
    files.push_back(base_path + "/cars/Volkswagen/Jetta.txt");
    for (size_t i = 0; i < files.size(); i++) {
      std::string & file = files[i];
      filesystem::NormalizePath(file);

      bool success = ra::testing::CreateFile(file.c_str());
      if (!success)
        return false;
    }

    return true;
  }

#if defined(__linux__) || defined(__APPLE__)
  bool Truncate(const char * file_path, uint64_t size) {
    //truncate -s 10737418240 10Gigfile.img
    
    std::string truncate_path = ra::filesystem::FindFileFromPaths("truncate");
    if (truncate_path.empty())
      return false;

    std::string current_dir = ra::filesystem::GetCurrentDirectory();

    //Prepare command arguments
    ra::strings::StringVector arguments;
    arguments.push_back("-s");
    arguments.push_back(ra::strings::ToString(size));
    arguments.push_back(file_path);

    //Run the new executable
    ra::process::processid_t pid = ra::process::StartProcess(truncate_path, current_dir, arguments);
    if (pid == ra::process::INVALID_PROCESS_ID)
      return false;

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    if (!wait_ok)
      return false;

    if (exit_code != 0)
      return false;
    return true;
  }
#endif
  
  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testNormalizePath) {
#ifdef _WIN32
    //test properly formatted path
    {
      static const std::string EXPECTED = "C:\\temp\\foo\\bar.txt";
      std::string path = EXPECTED;
      filesystem::NormalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }

    //test incorrectly formatted path
    {
      static const std::string EXPECTED = "C:\\temp\\foo\\bar.txt";
      std::string path = "C:/temp/foo/bar.txt";
      filesystem::NormalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }
#else
    //test properly formatted path
    {
      static const std::string EXPECTED = "/tmp/foo/bar.txt";
      std::string path = EXPECTED;
      filesystem::NormalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }

    //test incorrectly formatted path
    {
      static const std::string EXPECTED = "/tmp/foo/bar.txt";
      std::string path = "\\tmp\\foo\\bar.txt";
      filesystem::NormalizePath(path);
      ASSERT_EQ(EXPECTED, path);
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileSize) {
    //test NULL
    {
      const char * path = NULL;
      uint32_t size = filesystem::GetFileSize(path);
      ASSERT_EQ(0, size);
    }

    //test actual value
    {
      //create dummy file
      std::string filename = ra::testing::GetTestQualifiedName();
      ASSERT_TRUE(ra::testing::CreateFile(filename.c_str()));

#ifdef _WIN32
      static const uint32_t EXPECTED = 14;
#elif defined(__linux__) || defined(__APPLE__)
      static const uint32_t EXPECTED = 11;
#endif

      //test `const char *` api
      uint32_t size = filesystem::GetFileSize(filename.c_str());
      ASSERT_EQ(EXPECTED, size);
      size = 0;

      //test `FILE*` api
      FILE * ptr = fopen(filename.c_str(), "r");
      ASSERT_TRUE(ptr != NULL);
      size = filesystem::GetFileSize(ptr);
      fclose(ptr);
      ASSERT_EQ(EXPECTED, size);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileSize64) {
    //test NULL
    {
      const char * path = NULL;
      uint64_t size = filesystem::GetFileSize64(path);
      ASSERT_EQ(0, size);
    }

    //test actual value
    {
      //create dummy file
      std::string filename = ra::testing::GetTestQualifiedName();
      ASSERT_TRUE(ra::testing::CreateFile(filename.c_str()));

#ifdef _WIN32
      static const uint64_t EXPECTED = 14;
#elif defined(__linux__) || defined(__APPLE__)
      static const uint64_t EXPECTED = 11;
#endif

      uint64_t size = filesystem::GetFileSize64(filename.c_str());
      ASSERT_EQ(EXPECTED, size);
      size = 0;

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }

    //test large values
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      std::string filename = ra::testing::GetTestQualifiedName();
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      uint64_t actual_size = filesystem::GetFileSize64(filename.c_str());
      ASSERT_EQ(expected_size, actual_size);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFilename) {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string filename = filesystem::GetFilename(NULL);
      ASSERT_EQ(EXPECTED, filename);
    }

    //test filename only
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::GetFilename("foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (unix style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::GetFilename("/home/myDirectory/foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (windows style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::GetFilename("C:\\Users\\Antoine\\Desktop\\myDirectory\\foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFilenameWithoutExtension) {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string filename = filesystem::GetFilenameWithoutExtension(NULL);
      ASSERT_EQ(EXPECTED, filename);
    }

    //test filename only
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::GetFilenameWithoutExtension("foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (unix style)
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::GetFilenameWithoutExtension("/home/myDirectory/foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (windows style)
    {
      static const std::string EXPECTED = "foo";
      std::string filename = filesystem::GetFilenameWithoutExtension("C:\\Users\\Antoine\\Desktop\\myDirectory\\foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFileExists) {
    //test NULL
    {
      bool exists = filesystem::FileExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::FileExists("foo.bar.notfound.bang");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //create dummy file
      std::string filename = ra::testing::GetTestQualifiedName();
      ASSERT_TRUE(ra::testing::CreateFile(filename.c_str()));

      bool exists = filesystem::FileExists(filename.c_str());
      ASSERT_TRUE(exists);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFileExistsLargeFile) {
    //test with large files
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      const std::string SPACE = std::string(" ");
      const std::string EMPTY;
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      //test if file exists
      ASSERT_TRUE(filesystem::FileExists(filename.c_str())) << "File not found: " << filename;

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFindFiles) {
    //test NULL
    {
      ra::strings::StringVector files;
      bool success = filesystem::FindFiles(files, NULL);
      ASSERT_FALSE(success);
    }

    //test current directory
    {
      ra::strings::StringVector files;
      bool success = filesystem::FindFiles(files, ".", -1);
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0);
    }

    //create cars directory tree
    std::string basePath = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
    {
      bool carsOK = CreateCarsDirectory(basePath);
      ASSERT_TRUE(carsOK);
    }

    //test subdirectory
    {
      ra::strings::StringVector files;
      bool success = filesystem::FindFiles(files, basePath.c_str());
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0);

      //search for known values
      bool hasMazdaDirectory = false; //test finding empty directories
      bool hasHondaDirectory = false; //test non-empty directory
      bool hasPricesFile = false; //test for non-leaf files
      bool hasJettaFile = false; //test last filename
      for (size_t i = 0; i < files.size(); i++) {
        const std::string & entry = files[i];
        if (entry.find("Mazda") != std::string::npos) {
          hasMazdaDirectory = true;
        }
        else if (entry.find("Honda") != std::string::npos) {
          hasHondaDirectory = true;
        }
        else if (entry.find("prices.txt") != std::string::npos) {
          hasPricesFile = true;
        }
        else if (entry.find("Jetta") != std::string::npos) {
          hasJettaFile = true;
        }
      }
      ASSERT_TRUE(hasMazdaDirectory);
      ASSERT_TRUE(hasHondaDirectory);
      ASSERT_TRUE(hasPricesFile);
      ASSERT_TRUE(hasJettaFile);
    }

    //test depth
    {
      ra::strings::StringVector files;
      bool success = filesystem::FindFiles(files, basePath.c_str(), 1); //cars directories is found at level 1, cars direct subdirectory and files are found at level 0.
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0);

      //search for known values
      bool hasMazdaDirectory = false; //test finding empty directories
      bool hasHondaDirectory = false; //test non-empty directory
      bool hasPricesFile = false; //test for non-leaf files, leafs at level 1
      bool hasJettaFile = false; //test for filenames at depth 2
      for (size_t i = 0; i < files.size(); i++) {
        const std::string & entry = files[i];
        if (entry.find("Mazda") != std::string::npos) {
          hasMazdaDirectory = true;
        }
        else if (entry.find("Honda") != std::string::npos) {
          hasHondaDirectory = true;
        }
        else if (entry.find("prices.txt") != std::string::npos) {
          hasPricesFile = true;
        }
        else if (entry.find("Jetta") != std::string::npos) {
          hasJettaFile = true;
        }
      }
      ASSERT_TRUE(hasMazdaDirectory);
      ASSERT_TRUE(hasHondaDirectory);
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
      bool success = filesystem::FindFiles(files, path, 1);
      ASSERT_TRUE(success);
      ASSERT_GT(files.size(), (size_t)0);

      //assert normalization
#ifdef _WIN32
      ASSERT_EQ(files[0].find("C:\\\\"), std::string::npos); //assert that C:\\ (double backslashes) is not found
#else
      ASSERT_EQ(files[0].find("//"), std::string::npos); //assert that // (double slashes) is not found
#endif

      //search for the last (almost) directory of the root file system.
#ifdef _WIN32
      const std::string pattern = environment::GetEnvironmentVariable("windir"); //returns C:\Windows or C:\windows
#else
      const std::string pattern = "/var";
#endif
      //find the exact pattern in the list
      bool found = false;
      for (size_t i = 0; i < files.size(); i++) {
        const std::string & file = files[i];
        found |= (file == pattern);

#ifdef _WIN32
        //fix for multiple Windows system (including AppVeyor) which has 'windir' defined as 'C:\windows' instead of 'C:\Windows'.
        if (pattern == "C:\\windows" || pattern == "C:\\WINDOWS") {
          found |= (file == "C:\\Windows");
        }
#endif
      }
      ASSERT_TRUE(found) << "Failed finding the pattern '" << pattern.c_str() << "' in directory '" << path << "'.\n"
        "Found the following elements: \n" <<
        strings::Join(files, "\n").c_str();
    }

    //cleanup
    ra::filesystem::DeleteDirectory(basePath.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFindFileFromPaths) {
    //test no result
    {
      ra::strings::StringVector locations;
      bool success = ra::filesystem::FindFileFromPaths("a file that cannot be found", locations);
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
      bool success = ra::filesystem::FindFileFromPaths(filename, locations);
      ASSERT_TRUE(success);
      ASSERT_GE(locations.size(), 1) << "Failed to find the location of file '" << filename << "' in PATH."; //note allow finding more than 1 path for filename
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testDirectoryExists) {
    //test NULL
    {
      bool exists = filesystem::DirectoryExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::DirectoryExists("/home/fooBAR");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //get dummy directory
      std::string currentDirectory = filesystem::GetCurrentDirectory();
      ASSERT_TRUE(!currentDirectory.empty());

      bool exists = filesystem::DirectoryExists(currentDirectory.c_str());
      ASSERT_TRUE(exists);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testCreateDirectory) {
    //test NULL
    {
      bool success = filesystem::CreateDirectory(NULL);
      ASSERT_FALSE(success);
    }

    //test current directory
    {
      bool success = filesystem::CreateDirectory("."); //should return true as the directory already exists
      ASSERT_TRUE(success);
    }

    //test directory already exits (call twice)
    {
      std::string path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
      bool success = false;
      success = filesystem::CreateDirectory(path.c_str());
      ASSERT_TRUE(success);

      //call CreateDirectory() twice should still be a success
      success = filesystem::CreateDirectory(path.c_str());
      ASSERT_TRUE(success);

      //cleanup
      filesystem::DeleteDirectory(path.c_str());
    }

    //test subdirectories
    {
      //build path with subdirectories
      char separator[] = { GetPathSeparator(), '\0' };
      std::string base_path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
      std::string full_path = base_path;
      full_path << separator << "1" << separator << "2" << separator << "3" << separator << "4" << separator << "5";

      bool success = false;
      success = filesystem::CreateDirectory(full_path.c_str());
      ASSERT_TRUE(success);

      //cleanup
      filesystem::DeleteDirectory(base_path.c_str());
    }

    //test issue #27
    {
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();

      //build path with subdirectories
      const char * separator = GetPathSeparatorStr();
      std::string base_path = temp_dir + separator + ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
      std::string full_path = base_path;
      full_path << separator << "1" << separator << "2" << separator << "3" << separator << "4" << separator << "5";

      bool success = false;
      success = filesystem::CreateDirectory(full_path.c_str());
      ASSERT_TRUE(success);

      //cleanup
      filesystem::DeleteDirectory(base_path.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testDeleteFile) {
    //test NULL
    {
      bool success = filesystem::DeleteFile(NULL);
      ASSERT_FALSE(success);
    }

    //test success
    {
      std::string path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__) + ".txt";
      bool success = ra::testing::CreateFile(path.c_str());
      ASSERT_TRUE(success);

      success = filesystem::DeleteFile(path.c_str());
      ASSERT_TRUE(success);

      //assert file is actually deleted
      bool found = filesystem::FileExists(path.c_str());
      ASSERT_FALSE(found);
    }

    //test failure
    {
#ifndef _WIN32
      return; //test failure unsupported. See below.
#endif

      std::string path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__) + ".txt";
      bool success = ra::testing::CreateFile(path.c_str());
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

      success = filesystem::DeleteFile(path.c_str());
      ASSERT_FALSE(success);

#ifndef _WIN32
      //unset the 'immutable' flag allow the file to be deleted.
      //flags = 0;
      //ioctl(fileno(f), FS_IOC_SETFLAGS, &flags); //unset the immutable flag
#endif

      //release the file
      fclose(f);

      //try to delete again
      success = filesystem::DeleteFile(path.c_str());
      ASSERT_TRUE(success);

      //assert the file is really deleted
      bool found = filesystem::FileExists(path.c_str());
      ASSERT_FALSE(found);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testDeleteDirectory) {
    //test NULL
    {
      bool success = filesystem::DeleteDirectory(NULL);
      ASSERT_FALSE(success);
    }

    //create cars directory tree
    std::string basePath = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
    {
      bool carsOK = CreateCarsDirectory(basePath);
      ASSERT_TRUE(carsOK);
    }

    //test success
    {
      bool success = filesystem::DeleteDirectory(basePath.c_str());
      ASSERT_TRUE(success);

      //assert directory is actually deleted
      ASSERT_FALSE(filesystem::DirectoryExists(basePath.c_str()));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFileName) {
    //test not empty
    {
      std::string filename = filesystem::GetTemporaryFileName();
      ASSERT_TRUE(!filename.empty());
    }

    //test repetitive
    {
      std::vector<std::string> filenames;
      static const size_t numTest = 20;
      for (size_t i = 0; i < numTest; i++) {
        std::string filename = filesystem::GetTemporaryFileName();
        filenames.push_back(filename);
      }

      //assert that all values are unique
      for (size_t i = 0; i < numTest; i++) {
        const std::string & filename = filenames[i];
        int count = CountValues(filenames, filename);
        ASSERT_EQ(1, count) << "Found value '" << filename << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFilePath) {
    //test not empty
    {
      std::string path = filesystem::GetTemporaryFilePath();
      ASSERT_TRUE(!path.empty());
    }

    //test repetitive
    {
      std::vector<std::string> paths;
      static const size_t numTest = 20;
      for (size_t i = 0; i < numTest; i++) {
        std::string path = filesystem::GetTemporaryFilePath();
        paths.push_back(path);
      }

      //assert that all values are unique
      for (size_t i = 0; i < numTest; i++) {
        const std::string & path = paths[i];
        int count = CountValues(paths, path);
        ASSERT_EQ(1, count) << "Found value '" << path << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryDirectory) {
    std::string temp_dir = filesystem::GetTemporaryDirectory();
    ASSERT_TRUE(!temp_dir.empty());
    ASSERT_TRUE(ra::filesystem::DirectoryExists(temp_dir.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetParentPath) {
    //test no directory
    {
      static const std::string EXPECTED = "";
      std::string parent = filesystem::GetParentPath("filename.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test unix style
    {
      static const std::string EXPECTED = "/home/myDirectory";
      std::string parent = filesystem::GetParentPath("/home/myDirectory/foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test windows style
    {
      static const std::string EXPECTED = "C:\\Users\\Antoine\\Desktop\\myDirectory";
      std::string parent = filesystem::GetParentPath("C:\\Users\\Antoine\\Desktop\\myDirectory\\foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetShortPathForm) {
    //test spaces in filename
    {
      static const std::string EXPECTED = "ABC~1.TXT";
      std::string shortPath = filesystem::GetShortPathForm("a b c.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long file extension
    {
      static const std::string EXPECTED = "ABCDEF~1.TEX";
      std::string shortPath = filesystem::GetShortPathForm("abcdefgh.text");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long filename
    {
      static const std::string EXPECTED = "ABCDEF~1.TXT";
      std::string shortPath = filesystem::GetShortPathForm("abcdefghijklmnopqrstuvwxyz.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test spaces in file extension
    {
      static const std::string EXPECTED = "ABCDE~1.TXT";
      std::string shortPath = filesystem::GetShortPathForm("abcde.t x t");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test program files (windows style)
    {
      static const std::string EXPECTED = "PROGRA~1";
      std::string shortPath = filesystem::GetShortPathForm("Program Files (x86)");
      ASSERT_EQ(EXPECTED, shortPath);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPath) {
    //test baseline
    {
      static const std::string EXPECTED_PARENT = "/home/myDirectory";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::SplitPath("/home/myDirectory/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test empty
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "";
      std::string parent, filename;
      filesystem::SplitPath("", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test single filename
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "myfile.txt";
      std::string parent, filename;
      filesystem::SplitPath("myfile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test spaces in directory
    {
      static const std::string EXPECTED_PARENT = "/home/my Directory";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::SplitPath("/home/my Directory/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test filename without extension / directory name
    {
      static const std::string EXPECTED_PARENT = "/home/myDirectory";
      static const std::string EXPECTED_FILENAME = "myFile";
      std::string parent, filename;
      filesystem::SplitPath("/home/myDirectory/myFile", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    std::string directory;
    std::string filename;

    //from full path
    {
      SplitPath("C:\\foo\\bar\\file.txt", directory, filename);
      ASSERT_EQ(directory, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      SplitPath("C:\\foo\\bar\\file", directory, filename);
      ASSERT_EQ(directory, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file");
    }
    {
      SplitPath("C:\\foo\\bar\\file.", directory, filename);
      ASSERT_EQ(directory, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.");
    }

    //from filename only
    {
      SplitPath("file.txt", directory, filename);
      ASSERT_EQ(directory, "");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      SplitPath("file.", directory, filename);
      ASSERT_EQ(directory, "");
      ASSERT_EQ(filename, "file.");
    }
    {
      SplitPath("file", directory, filename);
      ASSERT_EQ(directory, "");
      ASSERT_EQ(filename, "file");
    }

    //empty strings
    {
      SplitPath("", directory, filename);
      ASSERT_EQ(directory, "");
      ASSERT_EQ(filename, "");
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPathArray) {
    //test baseline
    {
      std::vector<std::string> elements;
      filesystem::SplitPath("/home/myDirectory/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myDirectory", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test empty
    {
      std::vector<std::string> elements;
      filesystem::SplitPath("", elements);
      ASSERT_EQ(0, elements.size());
    }

    //test single filename
    {
      std::vector<std::string> elements;
      filesystem::SplitPath("myfile.txt", elements);
      ASSERT_EQ(1, elements.size());
      ASSERT_EQ("myfile.txt", elements[0]);
    }

    //test spaces in directory
    {
      std::vector<std::string> elements;
      filesystem::SplitPath("/home/my Directory/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("my Directory", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test filename without extension / directory name
    {
      std::vector<std::string> elements;
      filesystem::SplitPath("/home/myDirectory/myFile", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myDirectory", elements[1]);
      ASSERT_EQ("myFile", elements[2]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathSeparator) {
#ifdef _WIN32
    ASSERT_EQ('\\', filesystem::GetPathSeparator());
#elif defined(__linux__) || defined(__APPLE__)
    ASSERT_EQ('/', filesystem::GetPathSeparator());
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testMakeRelativePath) {
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
      std::string relative_path = ra::filesystem::MakeRelativePath(base_path, test_path);
      ASSERT_EQ(expected_path, relative_path);
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
      std::string relative_path = ra::filesystem::MakeRelativePath(base_path, test_path);
      ASSERT_EQ(expected_path, relative_path);
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
      std::string relative_path = ra::filesystem::MakeRelativePath(base_path, test_path);
      ASSERT_EQ(expected_path, relative_path);
    }

#ifdef _WIN32
    //test unrelated path
    {
      const std::string base_path = "C:\\Program Files\\Microsoft Office";
      const std::string test_path = "D:\\temp";
      std::string relative_path = ra::filesystem::MakeRelativePath(base_path, test_path);
      ASSERT_TRUE(relative_path.empty()) << "relative_path expected to be empty string but is '" << relative_path << "'.";
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetCurrentDirectory) {
    std::string curdir = GetCurrentDirectory();
    ASSERT_NE("", curdir);

    ASSERT_TRUE(filesystem::DirectoryExists(curdir.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileExtention) {
    //test baseline
    {
      static const std::string EXPECTED = "txt";
      std::string ext = filesystem::GetFileExtention("myFile.txt");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test empty
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::GetFileExtention("");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test spaces in extension
    {
      static const std::string EXPECTED = "foo bar";
      std::string ext = filesystem::GetFileExtention("/home/my Directory/myFile.foo bar");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test filename without extension / directory name
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::GetFileExtention("/home/myDirectory/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test directory with an extension and file without extension
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::GetFileExtention("/home/my.Directory/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //from valid filename
    {
      std::string ext = GetFileExtention("file.txt");
      ASSERT_EQ(ext, "txt");
    }
    {
      std::string ext = GetFileExtention("file.");
      ASSERT_EQ(ext, "");
    }
    {
      std::string ext = GetFileExtention("file");
      ASSERT_EQ(ext, "");
    }

    //from full path
    {
      std::string ext = GetFileExtention("C:\\foo\\bar\\file.txt");
      ASSERT_EQ(ext, "txt");
    }

    //from filename with multiple dots
    {
      std::string ext = GetFileExtention("file.subfile.txt");
      ASSERT_EQ(ext, "txt");
    }

    //empty strings
    {
      std::string ext = GetFileExtention("");
      ASSERT_EQ(ext, "");
    }

    //from special case path
    {
      std::string ext = GetFileExtention("C:\\foo.bar\\file");
      ASSERT_EQ(ext, "");
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetUserFriendlySize) {
    static const uint64_t MULTIPLICATOR_KB = 1024;
    static const uint64_t MULTIPLICATOR_MB = 1024 * MULTIPLICATOR_KB;
    static const uint64_t MULTIPLICATOR_GB = 1024 * MULTIPLICATOR_MB;
    static const uint64_t MULTIPLICATOR_TB = 1024 * MULTIPLICATOR_GB;

    {
      static const std::string EXPECTED = "0 bytes";
      std::string size = filesystem::GetUserFriendlySize(0ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1023 bytes";
      std::string size = filesystem::GetUserFriendlySize(1023ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 KB";
      std::string size = filesystem::GetUserFriendlySize(1ull * MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 MB";
      std::string size = filesystem::GetUserFriendlySize(1000ull * MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 MB";
      std::string size = filesystem::GetUserFriendlySize(1ull * MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 GB";
      std::string size = filesystem::GetUserFriendlySize(1000ull * MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 GB";
      std::string size = filesystem::GetUserFriendlySize(1ull * MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 TB";
      std::string size = filesystem::GetUserFriendlySize(1000ull * MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 TB";
      std::string size = filesystem::GetUserFriendlySize(1ull * MULTIPLICATOR_TB);
      ASSERT_EQ(EXPECTED, size);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileModifiedDate) {
    //assert that unit of return value is seconds
    {
      static const uint64_t EXPECTED = 3;
      const std::string filename1 = ra::testing::GetTestQualifiedName() + ".1.txt";
      const std::string filename2 = ra::testing::GetTestQualifiedName() + ".2.txt";

      //synchronize to the beginning of a new second on wall-clock.
      ra::timing::WaitNextSecond();
      ra::timing::Millisleep(100); // wait a little before trying to make a time based operation

      //create first file
      ASSERT_TRUE(ra::testing::CreateFile(filename1.c_str()));

      //allow 3 seconds between the files
      for (uint64_t i = 0; i < EXPECTED; i++) {
        ra::timing::WaitNextSecond();
        ra::timing::Millisleep(100); // wait a little before trying to make a time based operation
      }

      //create second file
      ASSERT_TRUE(ra::testing::CreateFile(filename2.c_str()));

      uint64_t time1 = filesystem::GetFileModifiedDate(filename1);
      uint64_t time2 = filesystem::GetFileModifiedDate(filename2);
      uint64_t diff = time2 - time1;
      ASSERT_GE(diff, EXPECTED);

      //cleanup
      ra::filesystem::DeleteFile(filename1.c_str());
      ra::filesystem::DeleteFile(filename2.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileModifiedDateLargeFile) {
    //test with large files
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      const std::string SPACE = std::string(" ");
      const std::string EMPTY;
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      //test file modified date
      uint64_t time = filesystem::GetFileModifiedDate(filename);
      ASSERT_NE(0, time);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testIsDirectoryEmpty) {
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmpty(""));

    //test with an empty directory
    const std::string test_dir_name = ra::testing::GetTestQualifiedName();
    ASSERT_TRUE(ra::filesystem::CreateDirectory(test_dir_name.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmpty(test_dir_name));
    
    //test with a sub directory
    const std::string sub_dir_path = test_dir_name + ra::filesystem::GetPathSeparatorStr() + "subdir";
    ASSERT_TRUE(ra::filesystem::CreateDirectory(sub_dir_path.c_str()));
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmpty(test_dir_name));
    
    //cleanup (sub directory)
    ASSERT_TRUE(ra::filesystem::DeleteDirectory(sub_dir_path.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmpty(test_dir_name));

    //test with a file in directory
    const std::string file_path = test_dir_name + ra::filesystem::GetPathSeparatorStr() + "file.tmp";
    ASSERT_TRUE(ra::filesystem::WriteTextFile(file_path, "content of the file"));
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmpty(test_dir_name));
    
    //cleanup (file)
    ASSERT_TRUE(ra::filesystem::DeleteFile(file_path.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmpty(test_dir_name));

    //cleanup (test directory)
    ra::filesystem::DeleteDirectory(test_dir_name.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasFileReadAccess) {
    //test NULL
    {
      bool result = filesystem::HasFileReadAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test for read access
    {
      std::string path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
      ASSERT_TRUE(ra::testing::CreateFile(path.c_str()));

      bool has_read = filesystem::HasFileReadAccess(path.c_str());
      ASSERT_TRUE(has_read);

      //cleanup
      filesystem::DeleteFile(path.c_str());
    }

    //test read access denied
    {
#ifdef _WIN32
      //not supported. Cannot find a file that exists but cannot be read.
      //Note, the file 'C:\pagefile.sys' can be found using FindFirstFile() but not with _stat() which I don't understand.
      //Note, based on #71, this problem is certainly because the file 'C:\pagefile.sys' was >= 4GB.
      return;
#elif defined(__linux__)
      const char * path = "/proc/sysrq-trigger"; //permission denied file
      ASSERT_TRUE(filesystem::FileExists(path));
      bool hasRead = filesystem::HasFileReadAccess(path);
      ASSERT_FALSE(hasRead);
#elif defined(__APPLE__)
      // Not able to find a file without read access on macos.
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasFileReadAccessLargeFile) {
    //test with large files
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      const std::string SPACE = std::string(" ");
      const std::string EMPTY;
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      //test for read access
      bool has_read = ra::filesystem::HasFileReadAccess(filename.c_str());
      ASSERT_TRUE(has_read);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasFileWriteAccess) {
    //test NULL
    {
      bool result = filesystem::HasFileWriteAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test for write access
    {
      std::string path = ra::testing::GetTestQualifiedName() + "." + ra::strings::ToString(__LINE__);
      ASSERT_TRUE(ra::testing::CreateFile(path.c_str()));

      bool has_write = filesystem::HasFileWriteAccess(path.c_str());
      ASSERT_TRUE(has_write);

      //cleanup
      filesystem::DeleteFile(path.c_str());
    }

    //test write access denied
    {
      std::string path;
#ifdef _WIN32
      //On Windows 7 and Windows 10, use "C:\\bootmgr"
      //On Github Actions, "C:\\bootmgr" is not available. Use "C:\Windows\WindowsShell.Manifest" instead.
      if (path.empty() && ra::filesystem::FileExists("C:\\bootmgr")) path = "C:\\bootmgr";
      if (path.empty() && ra::filesystem::FileExists("C:\\Windows\\WindowsShell.Manifest")) path = "C:\\Windows\\WindowsShell.Manifest";
#elif defined(__linux__)
      path = "/proc/cpuinfo"; //permission denied file
#elif defined(__APPLE__)
      path = "/etc/bashrc"; //permission denied file
      //path = "/etc/profile"; //permission denied file
#endif
      ASSERT_TRUE(filesystem::FileExists(path.c_str())) << "File '" << path << "' not found. Unable to call HasFileWriteAccess().";
      bool has_write = filesystem::HasFileWriteAccess(path.c_str());
      ASSERT_FALSE(has_write);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasFileWriteAccessLargeFile) {
    //test with large files
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      const std::string SPACE = std::string(" ");
      const std::string EMPTY;
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      //test for write access
      bool has_write = ra::filesystem::HasFileWriteAccess(filename.c_str());
      ASSERT_TRUE(has_write);

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasDirectoryReadAccess) {
    //test NULL
    {
      bool result = filesystem::HasDirectoryReadAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test for read access
    {
      std::string curr_dir = ra::filesystem::GetCurrentDirectory();
      bool has_read = filesystem::HasDirectoryReadAccess(curr_dir.c_str());
      ASSERT_TRUE(has_read) << "Directory '" << curr_dir << "' was expected to have read access.";
    }

    //No test for read denied access
    //Not supported. Cannot find a directory that exists but cannot be read.

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testHasDirectoryWriteAccess) {
    //test NULL
    {
      bool result = filesystem::HasDirectoryWriteAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test for write access
    {
      std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
      bool has_write = filesystem::HasDirectoryWriteAccess(temp_dir.c_str());
      ASSERT_TRUE(has_write) << "Directory '" << temp_dir << "' was expected to have write access.";
    }

    //test write access denied
    {
      std::string dir_path;
#ifdef _WIN32
      dir_path = ra::environment::GetEnvironmentVariable("windir");
      if (!ra::filesystem::DirectoryExists(dir_path.c_str()))
      {
        dir_path = "C:\\Windows";
      }
#elif defined(__linux__)
      dir_path = "/proc"; //permission denied file directory
#elif defined(__APPLE__)
      dir_path = "/Library/Printers"; //permission denied file directory
#endif
      ASSERT_TRUE(filesystem::DirectoryExists(dir_path.c_str())) << "Directory '" << dir_path << "' not found. Unable to call HasDirectoryWriteAccess().";
      bool has_write = filesystem::HasDirectoryWriteAccess(dir_path.c_str());
      ASSERT_FALSE(has_write) << "Directory '" << dir_path << "' was expected to NOT have write access.";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testAbsolutePath) {
#ifdef _WIN32
    ASSERT_TRUE(ra::filesystem::IsAbsolutePath("C:\\boot.ini"));
    ASSERT_TRUE(ra::filesystem::IsAbsolutePath("\\\\filesrv1\\public\\list.xml"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath("src\\main.cpp"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath(".\\src\\main.cpp"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath("..\\src\\main.cpp"));
#elif defined(__linux__) || defined(__APPLE__)
    ASSERT_TRUE(ra::filesystem::IsAbsolutePath("/home"));
    ASSERT_TRUE(ra::filesystem::IsAbsolutePath("/bin/bash"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath("src/main.cpp"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath(".//src/main.cpp"));
    ASSERT_FALSE(ra::filesystem::IsAbsolutePath("..//src/main.cpp"));
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathBasedOnCurrentProcess) {
    //test already absolute
    {
#ifdef _WIN32
      std::string testPath = "C:\\windows\\system32\\cmd.exe";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/bin/bash";
#elif __APPLE__
      std::string testPath = "/bin/zsh";
#endif
      std::string actual = ra::filesystem::GetPathBasedOnCurrentProcess(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }

    //test relative path
    {
      std::string testPath = "files\\images\\slashscreen.png";
      ra::filesystem::NormalizePath(testPath);

      std::string actual = ra::filesystem::GetPathBasedOnCurrentProcess(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }

    //test filename only
    {
      std::string testPath = "slashscreen.png";
      ra::filesystem::NormalizePath(testPath);

      std::string actual = ra::filesystem::GetPathBasedOnCurrentProcess(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathBasedOnCurrentDirectory) {
    //test already absolute
    {
#ifdef _WIN32
      std::string testPath = "C:\\windows\\system32\\cmd.exe";
#elif __linux__
      std::string testPath = "/bin/bash";
#elif __APPLE__
      std::string testPath = "/bin/zsh";
#endif
      std::string actual = ra::filesystem::GetPathBasedOnCurrentDirectory(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }

    //test relative path
    {
      std::string testPath = "files\\images\\slashscreen.png";
      ra::filesystem::NormalizePath(testPath);

      std::string actual = ra::filesystem::GetPathBasedOnCurrentDirectory(testPath);
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }

    //test filename only
    {
      std::string testPath = "slashscreen.png";
      ra::filesystem::NormalizePath(testPath);

      std::string actual = ra::filesystem::GetPathBasedOnCurrentDirectory(testPath);

      ASSERT_FALSE(actual.empty());
      ASSERT_NE(testPath, actual);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath01) {
    //test with ..
    {
      SCOPED_TRACE("test with ..");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\..\\baz\\myapp.exe";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/foo/bar/../baz/myapp";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\baz\\myapp.exe", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("/foo/baz/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath02) {
    //test with .. at the end
    {
      SCOPED_TRACE("test with .. at the end");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\..";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/foo/bar/..";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));

#ifdef _WIN32
      ASSERT_EQ("C:\\foo", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("/foo", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath03) {
    //test with .
    {
      SCOPED_TRACE("test with .");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\.\\baz\\myapp.exe";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/foo/bar/./baz/myapp";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("/./")); // . element removed from path
      ASSERT_EQ(std::string::npos, actual.find("\\.\\")); // . element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\bar\\baz\\myapp.exe", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("/foo/bar/baz/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath04) {
    //test with . at the end
    {
      SCOPED_TRACE("test with . at the end");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\bar\\.";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/foo/bar/.";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("/./")); // . element removed from path
      ASSERT_EQ(std::string::npos, actual.find("\\.\\")); // . element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));

#ifdef _WIN32
      ASSERT_EQ("C:\\foo\\bar", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("/foo/bar", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath05) {
    //test too many \..\ elements
    {
      SCOPED_TRACE("test too many \\..\\ elements");
#ifdef _WIN32
      std::string testPath = "C:\\foo\\..\\..\\..\\myapp.exe";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/foo/../../../myapp";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());
      ASSERT_EQ(std::string::npos, actual.find("..")); // .. element removed from path
      ASSERT_NE(actual, testPath);
      ASSERT_TRUE(ra::filesystem::IsAbsolutePath(actual));

#ifdef _WIN32
      ASSERT_EQ("C:\\myapp.exe", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath06) {
    //test with impossible to resolve situations (relative path)
    {
      SCOPED_TRACE("test with impossible to resolve situations (relative path)");
#ifdef _WIN32
      std::string testPath = "..\\foo\\..\\..\\bar\\baz\\..\\myapp.exe";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "../foo/../../bar/baz/../myapp";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());

#ifdef _WIN32
      ASSERT_EQ("..\\..\\bar\\myapp.exe", actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ("../../bar/myapp", actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testResolvePath07) {
    //test buggy path. See issue #11 https://github.com/end2endzone/RapidAssist/issues/11
    {
      SCOPED_TRACE("test buggy path");
#ifdef _WIN32
      std::string testPath = "\\home\\pi\\dev\\github\\RapidAssist\\build\\bin\\files\\images\\slashscreen.png";
#elif defined(__linux__) || defined(__APPLE__)
      std::string testPath = "/home/pi/dev/github/RapidAssist/build/bin/files/images/slashscreen.png";
#endif
      std::string actual = ra::filesystem::ResolvePath(testPath);
      SCOPED_TRACE(actual);

      ASSERT_FALSE(actual.empty());

#ifdef _WIN32
      ASSERT_EQ(testPath, actual);
#elif defined(__linux__) || defined(__APPLE__)
      ASSERT_EQ(testPath, actual);
#endif
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testIsRootDirectory) {
#ifdef _WIN32
    ASSERT_TRUE(ra::filesystem::IsRootDirectory("C:\\"));
    ASSERT_TRUE(ra::filesystem::IsRootDirectory("c:\\"));
    ASSERT_TRUE(ra::filesystem::IsRootDirectory("z:\\"));
    ASSERT_FALSE(ra::filesystem::IsRootDirectory("c:\\foo"));
    ASSERT_FALSE(ra::filesystem::IsRootDirectory("c:"));
#elif defined(__linux__) || defined(__APPLE__)
    ASSERT_TRUE(ra::filesystem::IsRootDirectory("/"));
    ASSERT_FALSE(ra::filesystem::IsRootDirectory("/foo"));
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testCopyFileBasic) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPath();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectory();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExists(process_path.c_str())) << "Process path '" << process_path << "' is not found!";
    ASSERT_TRUE(ra::filesystem::DirectoryExists(temp_dir.c_str())) << "Directory '" << temp_dir << "' is not found!";

    bool copied = ra::filesystem::CopyFile(process_path, output_path);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExists(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSize(output_path.c_str());
    ASSERT_EQ(source_size, target_size);
  }
  //--------------------------------------------------------------------------------------------------
  static bool gProgressBegin = false;
  static bool gProgressEnd = false;
  void myCopyFileCallbackFunction(double progress) {
    //remember first and last callbacks
    if (progress == 0.0)
      gProgressBegin = true;
    if (progress == 1.0)
      gProgressEnd = true;

    printf("%s(%.2f)\n", __FUNCTION__, progress);
  }
  TEST_F(TestFilesystem, testCopyFileCallbackFunction) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPath();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectory();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExists(process_path.c_str()));
    ASSERT_TRUE(ra::filesystem::DirectoryExists(temp_dir.c_str()));

    gProgressBegin = false;
    gProgressEnd = false;
    bool copied = ra::filesystem::CopyFile(process_path, output_path, &myCopyFileCallbackFunction);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExists(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSize(output_path.c_str());
    ASSERT_EQ(source_size, target_size);

    //assert that first and last progress was received
    ASSERT_TRUE(gProgressBegin);
    ASSERT_TRUE(gProgressEnd);
  }
  //--------------------------------------------------------------------------------------------------
  class CopyFileCallbackFunctor : public virtual ra::filesystem::IProgressReport {
  public:
    CopyFileCallbackFunctor() : progress_begin_(false), progress_end_(false) {};
    virtual ~CopyFileCallbackFunctor() {};
    virtual void OnProgressReport(double progress) {
      //remember first and last callbacks
      if (progress == 0.0)
        progress_begin_ = true;
      if (progress == 1.0)
        progress_end_ = true;

      printf("%s(%.2f)\n", __FUNCTION__, progress);
    }
    bool hasProgressBegin() { return progress_begin_; }
    bool hasProgressEnd() { return progress_end_; }
  private:
    bool progress_begin_;
    bool progress_end_;
  };
  TEST_F(TestFilesystem, testCopyFileCallbackFunctor) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPath();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectory();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExists(process_path.c_str()));
    ASSERT_TRUE(ra::filesystem::DirectoryExists(temp_dir.c_str()));

    CopyFileCallbackFunctor functor;
    bool copied = ra::filesystem::CopyFile(process_path, output_path, &functor);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExists(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSize(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSize(output_path.c_str());
    ASSERT_EQ(source_size, target_size);

    //assert that first and last progress was received
    ASSERT_TRUE(functor.hasProgressBegin());
    ASSERT_TRUE(functor.hasProgressEnd());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadFile) {
    //test file not found
    {
      const std::string file_path = "this file is not found";
      std::string content;
      bool success = ra::filesystem::ReadFile(file_path, content);
      ASSERT_FALSE(success);
    }

    //test file read
    {
      const std::string file_path = ra::process::GetCurrentProcessPath();
      std::string content;
      bool success = ra::filesystem::ReadFile(file_path, content);
      ASSERT_TRUE(success);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteFile) {
    //test write fail
    {
      const std::string file_path = ra::filesystem::GetCurrentDirectory() +
        ra::filesystem::GetPathSeparatorStr() + "foo" +
        ra::filesystem::GetPathSeparatorStr() + "bar" +
        ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName() + ".bin"; //directory 'foo\bar' does not exists.
      const size_t content_size = (size_t)ra::random::GetRandomInt(1300, 13000);
      const std::string content = ra::random::GetRandomString(content_size);
      bool success = ra::filesystem::WriteFile(file_path, content);
      ASSERT_FALSE(success);
    }

    //test random content write
    {
      const std::string file_path = ra::testing::GetTestQualifiedName() + ".bin";
      const size_t content_size = (size_t)ra::random::GetRandomInt(1300, 13000);
      const std::string content = ra::random::GetRandomString(content_size);
      bool success = ra::filesystem::WriteFile(file_path, content);
      ASSERT_TRUE(success);

      //assert write success
      const bool found = ra::filesystem::FileExists(file_path.c_str());
      const uint32_t write_size = ra::filesystem::GetFileSize(file_path.c_str());
      ASSERT_EQ(content_size, write_size);

      //cleanup
      ra::filesystem::DeleteFile(file_path.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadWriteFileDataLoss) {
    const std::string file_path = ra::testing::GetTestQualifiedName() + ".bin";
    const size_t content_size = (size_t)ra::random::GetRandomInt(1300, 13000);
    const std::string content_write = ra::random::GetRandomString(content_size);
    bool success = ra::filesystem::WriteFile(file_path, content_write);
    ASSERT_TRUE(success);

    //assert write success
    const uint32_t write_size = ra::filesystem::GetFileSize(file_path.c_str());
    ASSERT_EQ(content_size, write_size);

    std::string content_read;
    success = ra::filesystem::ReadFile(file_path, content_read);
    ASSERT_TRUE(success);

    //assert that we readed the same amount of data
    ASSERT_EQ(content_write.size(), content_read.size());
    ASSERT_EQ(content_write, content_read); //and the same data

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testReadTextFile) {
    const std::string newline = ra::environment::GetLineSeparator();
    const std::string content =
      "The" + newline +
      "quick" + newline +
      "brown" + newline +
      "fox" + newline +
      "jumps" + newline +
      "over" + newline +
      "the" + newline +
      "lazy" + newline +
      "dog.";
    const std::string file_path = ra::testing::GetTestQualifiedName() + ".txt";
    bool success = ra::filesystem::WriteFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //read as lines
    ra::strings::StringVector lines;
    bool readok = ReadTextFile(file_path, lines, true);
    ASSERT_TRUE(readok);

    //assert each lines properly readed
    static const size_t num_expected_lines = 9;
    ASSERT_EQ(num_expected_lines, lines.size());
    ASSERT_EQ(std::string("The"), lines[0]);
    ASSERT_EQ(std::string("quick"), lines[1]);
    ASSERT_EQ(std::string("brown"), lines[2]);
    ASSERT_EQ(std::string("fox"), lines[3]);
    ASSERT_EQ(std::string("jumps"), lines[4]);
    ASSERT_EQ(std::string("over"), lines[5]);
    ASSERT_EQ(std::string("the"), lines[6]);
    ASSERT_EQ(std::string("lazy"), lines[7]);
    ASSERT_EQ(std::string("dog."), lines[8]);

    //read as a single buffer
    std::string buffer;
    readok = ReadTextFile(file_path, buffer);
    ASSERT_TRUE(readok);

    //assert content is properly readed
#ifdef _WIN32
    // CRLF characters are replaced by CR when a file is readed in text mode
    std::string expected = content;
    ra::strings::Replace(expected, newline, "\n");
#else
    std::string expected = content;
#endif
    ASSERT_EQ(expected.size(), buffer.size());
    ASSERT_EQ(expected, buffer);

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteTextFileFromString) {
    const std::string newline = ra::environment::GetLineSeparator();
    const std::string content =
      "The" + newline +
      "quick" + newline +
      "brown" + newline +
      "fox" + newline +
      "jumps" + newline +
      "over" + newline +
      "the" + newline +
      "lazy" + newline +
      "dog.";
    const std::string file_path = ra::testing::GetTestQualifiedName() + ".txt";
    bool success = ra::filesystem::WriteFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //assert file size
#ifdef _WIN32
    const uint32_t expected_file_size = 52;
#else
    const uint32_t expected_file_size = 52 - 8; // 1 byte per newline less than windows version
#endif
    const uint32_t file_size = ra::filesystem::GetFileSize(file_path.c_str());
    ASSERT_EQ(expected_file_size, file_size);

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testWriteTextFileFromLines) {
    const std::string newline = ra::environment::GetLineSeparator();

    //building word list
    static const std::string sentence = "The quick brown fox jumps over the lazy dog.";
    ra::strings::StringVector word_list;
    ra::strings::Split(word_list, sentence, " ");

    //assert writing without adding new line between words
    const std::string file_path1 = ra::testing::GetTestQualifiedName() + ".1.txt";
    bool success = ra::filesystem::WriteTextFile(file_path1, word_list, false);
    ASSERT_TRUE(success);

    //read the generated file as a binary string
    std::string binary;
    bool readed = ra::filesystem::ReadFile(file_path1, binary);
    ASSERT_TRUE(readed);

    //assert no newline between words
    static const std::string expected_binary = "Thequickbrownfoxjumpsoverthelazydog.";
    ASSERT_EQ(expected_binary, binary);

    //assert file size
    {
      const uint32_t expected_file_size = 36;
      const uint32_t file_size = ra::filesystem::GetFileSize(file_path1.c_str());
      ASSERT_EQ(expected_file_size, file_size);
    }

    //assert writing with new line between words
    const std::string file_path2 = ra::testing::GetTestQualifiedName() + ".2.txt";
    success = ra::filesystem::WriteTextFile(file_path2, word_list, true);
    ASSERT_TRUE(success);

    //read the generated file as a binary string
    readed = ra::filesystem::ReadFile(file_path2, binary);
    ASSERT_TRUE(readed);

    //assert newline between words
    {
      const uint32_t minimum_file_size = 40;
      const uint32_t file_size = ra::filesystem::GetFileSize(file_path2.c_str());
      ASSERT_GT(file_size, minimum_file_size);
    }

    //cleanup
    ra::filesystem::DeleteFile(file_path1.c_str());
    ra::filesystem::DeleteFile(file_path2.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testPeekFile) {
    const std::string newline = ra::environment::GetLineSeparator();

    //building word list
    static const std::string sentence = "The quick brown fox jumps over the lazy dog.";

    //create a test file
    const std::string file_path = ra::testing::GetTestQualifiedName() + ".txt";

    //create a huge file
    std::string buffer;
    buffer.reserve(500 * (sentence.size() + 10)); //help speed the test a bit
    for (size_t i = 0; i < 500; i++) {
      buffer += sentence;
      buffer += "\n";
      buffer += ra::strings::ToString(i);
      buffer += "\n";
    }
    bool write_ok = ra::filesystem::WriteFile(file_path, buffer);
    ASSERT_TRUE(write_ok);
    buffer.clear();

    //try different peek size for testing
    {
      static const size_t peek_sizes[] = {
        1234,
        50,
        1,
        0,
      };
      const size_t num_peek_sizes = sizeof(peek_sizes) / sizeof(peek_sizes[0]);

      for (size_t i = 0; i < num_peek_sizes; i++) {
        const size_t peek_size = peek_sizes[i];
        std::string buffer;
        bool peek_ok = ra::filesystem::PeekFile(file_path, peek_size, buffer);
        ASSERT_TRUE(peek_ok) << "Failed peeking " << peek_size << " bytes into file '" << file_path << "'.";
        ASSERT_EQ(peek_size, buffer.size());
      }
    }

    //peek into the file
    {
      const size_t peek_size = 155;
      bool peek_ok = ra::filesystem::PeekFile(file_path, peek_size, buffer);
      ASSERT_TRUE(peek_ok);
      ASSERT_EQ(peek_size, buffer.size());

      //assert the expected content
      static const std::string expected = "The quick brown fox jumps over the lazy dog.\n0\nThe quick brown fox jumps over the lazy dog.\n1\nThe quick brown fox jumps over the lazy dog.\n2\nThe quick brow";
      ASSERT_EQ(peek_size, expected.size());
      ASSERT_EQ(expected.size(), buffer.size());
      ASSERT_EQ(expected, buffer);
    }

    //peek bigger than the file's size
    {
      const uint32_t file_size = (size_t)ra::filesystem::GetFileSize(file_path.c_str());
      const uint32_t peek_size = file_size + 10000;
      bool peek_ok = ra::filesystem::PeekFile(file_path, peek_size, buffer);
      ASSERT_TRUE(peek_ok);
      ASSERT_EQ(file_size, buffer.size());
    }

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testPeekFileLargeFile) {
    //test with large files
    static const uint64_t ONE_GB = 1073741824;
    static const uint64_t values[] = {
      1084,
      (uint64_t)(ONE_GB*1.10),
      (uint64_t)(ONE_GB*2.17),
      (uint64_t)(ONE_GB*3.23),
      (uint64_t)(ONE_GB*4.25),
    };
    static const size_t num_values = sizeof(values)/sizeof(values[0]);

    for(size_t i=0; i<num_values; i++)
    {
      const uint64_t & expected_size = values[i];

      std::string user_size = ra::filesystem::GetUserFriendlySize(expected_size);

      const std::string SPACE = std::string(" ");
      const std::string EMPTY;
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFile(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparse(filename.c_str(), expected_size);
#if defined(__linux__) || defined(__APPLE__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      //peek into the file
      {
        std::string buffer;

        const size_t peek_size = 1024;
        bool peek_ok = ra::filesystem::PeekFile(filename, peek_size, buffer);
        ASSERT_TRUE(peek_ok);
        ASSERT_EQ(peek_size, buffer.size());
      }

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFileReplace) {
    //create a test file
    static const std::string sentence = "The quick brown fox jumps over the lazy dog.";
    const std::string file_path = ra::testing::GetTestQualifiedName() + ".txt";
    bool write_ok = ra::filesystem::WriteTextFile(file_path, sentence);
    ASSERT_TRUE(write_ok);

    //process with search and replace
    bool replaced1 = ra::filesystem::FileReplace(file_path, "brown", "rainbow");
    bool replaced2 = ra::filesystem::FileReplace(file_path, "fox", "chameleon");
    bool replaced3 = ra::filesystem::FileReplace(file_path, "quick", "hungry");
    bool replaced4 = ra::filesystem::FileReplace(file_path, "jumps", "swings his tongue");
    bool replaced5 = ra::filesystem::FileReplace(file_path, "over the lazy dog", "from 0 to 60 miles per hour in a hundredth of a second");
    ASSERT_TRUE(replaced1);
    ASSERT_TRUE(replaced2);
    ASSERT_TRUE(replaced3);
    ASSERT_TRUE(replaced4);
    ASSERT_TRUE(replaced5);

    //read the replaced file
    std::string text;
    bool read_ok = ra::filesystem::ReadTextFile(file_path, text);
    ASSERT_TRUE(read_ok);
    static const std::string expected = "The hungry rainbow chameleon swings his tongue from 0 to 60 miles per hour in a hundredth of a second.";
    ASSERT_EQ(expected, text);

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace filesystem
} //namespace ra
