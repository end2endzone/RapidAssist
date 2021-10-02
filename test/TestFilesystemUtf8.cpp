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

#include "TestFilesystemUtf8.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing_utf8.h"
#include "rapidassist/timing.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/environment.h"
#include "rapidassist/environment_utf8.h"

namespace ra { namespace filesystem { namespace test
{

#if defined(__linux__) || defined(__APPLE__)
  extern bool Truncate(const char * file_path, uint64_t size);
#endif

  //--------------------------------------------------------------------------------------------------
  void TestFilesystemUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestFilesystemUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testFileExistsUtf8) {
    //test NULL
    {
      bool exists = filesystem::FileExistsUtf8(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::FileExistsUtf8("foo.bar.notfound.bang");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //create dummy file
      std::string filename = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";;
      ASSERT_TRUE(ra::testing::CreateFileUtf8(filename.c_str()));

      bool exists = filesystem::FileExistsUtf8(filename.c_str());
      ASSERT_TRUE(exists);

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testFileExistsLargeFileUtf8) {
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
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".psi_\xCE\xA8_psi.tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFileUtf8(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparseUtf8(filename.c_str(), expected_size);
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
      ASSERT_TRUE(filesystem::FileExistsUtf8(filename.c_str())) << "File not found: " << filename;

      //cleanup
      ra::filesystem::DeleteFile(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testWriteFileUtf8) {
    std::string filename = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";

    std::string content = std::string("Hello world from ") + ra::testing::GetTestQualifiedName();

    bool file_write = ra::filesystem::WriteTextFileUtf8(filename, content);
    ASSERT_TRUE(file_write);

#ifdef _WIN32
    //On Windows, the file should be not found using standard FileExist()
    bool found_win32 = ra::filesystem::FileExists(filename.c_str());
    ASSERT_FALSE(found_win32);
#endif

    bool found_utf8 = ra::filesystem::FileExistsUtf8(filename.c_str());
    ASSERT_TRUE(found_utf8);

    //cleanup
    ra::filesystem::DeleteFileUtf8(filename.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testGetFileSizeUtf8) {
    std::string filename = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";

    uint32_t file_length = 365;
    std::string content = std::string(file_length, '1');

    bool file_write = ra::filesystem::WriteTextFileUtf8(filename, content);
    ASSERT_TRUE(file_write);

#ifdef _WIN32
    //On Windows, the file should be not found using standard FileExist()
    bool found_win32 = ra::filesystem::FileExists(filename.c_str());
    ASSERT_FALSE(found_win32);
    uint32_t size_win32 = ra::filesystem::GetFileSize(filename.c_str());
    ASSERT_EQ(0, size_win32);
#endif

    uint32_t size_utf8 = ra::filesystem::GetFileSizeUtf8(filename.c_str());
    ASSERT_EQ(file_length, size_utf8);

    //cleanup
    ra::filesystem::DeleteFileUtf8(filename.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testGetFileSize64Utf8) {
    //test NULL
    {
      const char * path = NULL;
      uint64_t size = filesystem::GetFileSize64Utf8(path);
      ASSERT_EQ(0, size);
    }

    //test actual value
    {
      //create dummy file
      std::string filename = ra::testing::GetTestQualifiedName();
      ASSERT_TRUE(ra::testing::CreateFileUtf8(filename.c_str()));

#ifdef _WIN32
      static const uint64_t EXPECTED = 14;
#elif defined(__linux__) || defined(__APPLE__)
      static const uint64_t EXPECTED = 11;
#endif

      uint64_t size = filesystem::GetFileSize64Utf8(filename.c_str());
      ASSERT_EQ(EXPECTED, size);
      size = 0;

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
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
          ra::filesystem::DeleteFileUtf8(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparseUtf8(filename.c_str(), expected_size);
#if defined(__linux__)
      if (!created)
      {
        printf("Sparse file creation failed. Trying again with the 'truncate' command.\n");
        created = Truncate(filename.c_str(), expected_size);
        if (created)
          printf("Truncate command success. Resuming test execution.\n");
      }
#endif      
      ASSERT_TRUE( created ) << "Failed to create sparse file '" << filename << "'.";

      uint64_t actual_size = filesystem::GetFileSize64Utf8(filename.c_str());
      ASSERT_EQ(expected_size, actual_size);

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testHasFileReadWriteAccessUtf8) {
    std::string filename = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";

    uint32_t file_length = 365;
    std::string content = std::string(file_length, '1');

    bool file_write = ra::filesystem::WriteTextFileUtf8(filename, content);
    ASSERT_TRUE(file_write);

    bool have_read = ra::filesystem::HasFileReadAccessUtf8(filename.c_str());
    ASSERT_TRUE(have_read);

    bool have_write = ra::filesystem::HasFileWriteAccessUtf8(filename.c_str());
    ASSERT_TRUE(have_write);

#ifdef _WIN32
    //On Windows, the non-utf8 functions HasReadAccess() and HasWriteAccess() should not be working
    bool have_read_win32 = ra::filesystem::HasFileReadAccess(filename.c_str());
    ASSERT_FALSE(have_read_win32);
    bool have_write_win32 = ra::filesystem::HasFileWriteAccess(filename.c_str());
    ASSERT_FALSE(have_write_win32);
#endif

    //cleanup
    ra::filesystem::DeleteFileUtf8(filename.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testHasFileReadWriteAccessLargeFileUtf8) {
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
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".psi_\xCE\xA8_psi.tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFileUtf8(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparseUtf8(filename.c_str(), expected_size);
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

      //test read and write access
      bool have_read = ra::filesystem::HasFileReadAccessUtf8(filename.c_str());
      ASSERT_TRUE(have_read);

      bool have_write = ra::filesystem::HasFileWriteAccessUtf8(filename.c_str());
      ASSERT_TRUE(have_write);

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testHasDirectoryReadAccessUtf8) {
    //test NULL
    {
      bool result = filesystem::HasDirectoryReadAccessUtf8(NULL);
      ASSERT_FALSE(result);
    }

    //test for read access
    {
      std::string temp_dir = ra::filesystem::GetTemporaryDirectoryUtf8();

      static const std::string separator = ra::filesystem::GetPathSeparatorStr();
      std::string new_path = temp_dir + separator + ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi";
      bool created = ra::filesystem::CreateDirectoryUtf8(new_path.c_str());
      ASSERT_TRUE(created);

      bool has_read = filesystem::HasDirectoryReadAccessUtf8(new_path.c_str());
      ASSERT_TRUE(has_read);

      //cleanup
      bool deleted = ra::filesystem::DeleteDirectoryUtf8(new_path.c_str());
    }

    //No test for read denied access
    //Not supported. Cannot find a directory that exists but cannot be read.
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testHasDirectoryWriteAccessUtf8) {
    //test NULL
    {
      bool result = filesystem::HasDirectoryWriteAccess(NULL);
      ASSERT_FALSE(result);
    }

    //test for write access
    {
      std::string temp_dir = ra::filesystem::GetTemporaryDirectoryUtf8();
      bool has_write = filesystem::HasDirectoryWriteAccessUtf8(temp_dir.c_str());

      static const std::string separator = ra::filesystem::GetPathSeparatorStr();
      std::string new_path = temp_dir + separator + ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi";
      bool created = ra::filesystem::CreateDirectoryUtf8(new_path.c_str());
      ASSERT_TRUE(created);

      has_write = filesystem::HasDirectoryWriteAccessUtf8(new_path.c_str());
      ASSERT_TRUE(has_write);

      //cleanup
      bool deleted = ra::filesystem::DeleteDirectoryUtf8(new_path.c_str());
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
      ASSERT_TRUE(filesystem::DirectoryExists(dir_path.c_str())) << "Directory '" << dir_path << "' not found. Unable to call HasDirectoryWriteAccessUtf8().";
      bool has_write = filesystem::HasDirectoryWriteAccessUtf8(dir_path.c_str());
      ASSERT_FALSE(has_write);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testFindFilesUtf8) {
    //Create a working directory that matches current test name
    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string test_dir_name = ra::testing::GetTestQualifiedName();
    const std::string test_dir_path = ra::process::GetCurrentProcessDir() + separator + test_dir_name;
    bool success = filesystem::CreateDirectory(test_dir_path.c_str());
    ASSERT_TRUE(success);

    //create foo.txt
    std::string file_path1 = test_dir_path + separator + "foo.txt";
    bool file_created1 = ra::testing::CreateFile(file_path1.c_str());
    ASSERT_TRUE( file_created1 );

    //create utf-8 file
    std::string file_path2 = test_dir_path + separator + "psi_\xCE\xA8_psi.txt";
    bool file_created2 = ra::filesystem::WriteTextFileUtf8(file_path2, std::string(365, '1'));
    ASSERT_TRUE(file_created2);

    //create bar.txt
    std::string file_path3 = test_dir_path + separator + "bar.txt";
    bool file_created3 = ra::testing::CreateFile(file_path3.c_str());
    ASSERT_TRUE( file_created3 );

    //Search for files
    ra::strings::StringVector files;
    bool file_listed = ra::filesystem::FindFilesUtf8(files, test_dir_path.c_str());
    ASSERT_TRUE(file_listed);

    //One of the files must have an utf8 title
    bool found_utf8_file = false;
    for(size_t i=0; i<files.size(); i++)
    {
      const std::string & tmp_file_path = files[i];
      const std::string tmp_filename = ra::filesystem::GetFilename(tmp_file_path.c_str());
      if (tmp_filename == "psi_\xCE\xA8_psi.txt")
        found_utf8_file = true;
    }
    ASSERT_TRUE(found_utf8_file);

    //cleanup
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testFindFileFromPathsUtf8) {
    //Create a working directory that matches current test name
    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string test_dir_name = ra::testing::GetTestQualifiedName();
    const std::string test_dir_path = ra::process::GetCurrentProcessDir() + separator + test_dir_name;
    bool success = filesystem::CreateDirectory(test_dir_path.c_str());
    ASSERT_TRUE(success);

    //create utf-8 file
    std::string file_path = test_dir_path + separator + "omega_\xCE\xA9_omega.txt";
    bool file_created = ra::filesystem::WriteTextFileUtf8(file_path, std::string(365, '1'));
    ASSERT_TRUE(file_created);

    //Add test directory to PATH
#ifdef _WIN32
    const std::string path_separator = ";";
#else
    const std::string path_separator = ":";
#endif
    std::string path_old = ra::environment::GetEnvironmentVariableUtf8("PATH");
    std::string path_new = path_old + path_separator + test_dir_path;
    bool path_set = ra::environment::SetEnvironmentVariableUtf8("PATH", path_new.c_str());
    ASSERT_TRUE(path_set);

    //Search the utf-8 file
    std::string path_found = ra::filesystem::FindFileFromPathsUtf8("omega_\xCE\xA9_omega.txt");
    ASSERT_TRUE( !path_found.empty() );
    ASSERT_EQ(file_path, path_found);

    //cleanup
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testCreateDirectoryUtf8) {
    //create utf-8 directory
    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string process_dir_path = ra::process::GetCurrentProcessDir();
    const std::string test_dir_name = ra::testing::GetTestQualifiedName() + ".omega_\xCE\xA9_omega";
    std::string new_dir_path = process_dir_path + separator + test_dir_name;
    bool created = ra::filesystem::CreateDirectoryUtf8(new_dir_path.c_str());
    ASSERT_TRUE(created);
 
    bool exists = ra::filesystem::DirectoryExistsUtf8(new_dir_path.c_str());
    ASSERT_TRUE(exists);

#ifdef _WIN32
    //On Windows, the DirectoryExists() function should not be able to find the directory.
    bool have_dir_win32 = ra::filesystem::DirectoryExists(new_dir_path.c_str());
    ASSERT_FALSE(have_dir_win32);
#endif
 
    //cleanup
    ra::filesystem::DeleteDirectoryUtf8(new_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testDeleteDirectoryUtf8) {
    //create utf-8 directory
    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string process_dir_path = ra::process::GetCurrentProcessDir();
    const std::string test_dir_name = ra::testing::GetTestQualifiedName() + ".omega_\xCE\xA9_omega";
    const std::string new_dir_path = process_dir_path + separator + test_dir_name;
    bool created = ra::filesystem::CreateDirectoryUtf8(new_dir_path.c_str());
    ASSERT_TRUE(created);
 
    bool exists = ra::filesystem::DirectoryExistsUtf8(new_dir_path.c_str());
    ASSERT_TRUE(exists);
 
    //do the actual deletion
    bool deleted = ra::filesystem::DeleteDirectoryUtf8(new_dir_path.c_str());
    ASSERT_TRUE(deleted);
 
    exists = ra::filesystem::DirectoryExistsUtf8(new_dir_path.c_str());
    ASSERT_FALSE(exists);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testDeleteFileUtf8) {
    //create utf-8 file
    const std::string separator = ra::filesystem::GetPathSeparatorStr();
    const std::string process_dir_path = ra::process::GetCurrentProcessDir();
    const std::string test_file_name = ra::testing::GetTestQualifiedName() + ".omega_\xCE\xA9_omega.txt";
    const std::string test_file_path = process_dir_path + separator + test_file_name;
    bool file_write = ra::filesystem::WriteTextFileUtf8(test_file_path, std::string(300, '1'));
    ASSERT_TRUE(file_write);

#ifdef _WIN32
    //On Windows, the DeleteFile() function should not be able to delete the file
    bool deleted_win32 = ra::filesystem::DeleteFile(test_file_path.c_str());
    ASSERT_FALSE(deleted_win32);
#endif

    //do the actual file deletion
    bool deleted = ra::filesystem::DeleteFileUtf8(test_file_path.c_str());
    ASSERT_TRUE(deleted);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testGetFileModifiedDateUtf8) {
    //assert that unit of return value is seconds
    {
      static const uint64_t EXPECTED = 3;
      const std::string filename1 = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.1.txt";
      const std::string filename2 = ra::testing::GetTestQualifiedName() + ".omega_\xCE\xA9_omega.2.txt";

      //synchronize to the beginning of a new second on wall-clock.
      ra::timing::WaitNextSecond();
      ra::timing::Millisleep(100); // wait a little before trying to make a time based operation

      //create first file
      ASSERT_TRUE(ra::testing::CreateFileUtf8(filename1.c_str()));

      //allow 3 seconds between the files
      for (uint64_t i = 0; i < EXPECTED; i++) {
        ra::timing::WaitNextSecond();
        ra::timing::Millisleep(100); // wait a little before trying to make a time based operation
      }

      //create second file
      ASSERT_TRUE(ra::testing::CreateFileUtf8(filename2.c_str()));

      uint64_t time1 = filesystem::GetFileModifiedDateUtf8(filename1);
      uint64_t time2 = filesystem::GetFileModifiedDateUtf8(filename2);
      uint64_t diff = time2 - time1;
      ASSERT_GE(diff, EXPECTED);

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename1.c_str());
      ra::filesystem::DeleteFileUtf8(filename2.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testGetFileModifiedDateLargeFileUtf8) {
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
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".psi_\xCE\xA8_psi.tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFileUtf8(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparseUtf8(filename.c_str(), expected_size);
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
      uint64_t time = filesystem::GetFileModifiedDateUtf8(filename);
      ASSERT_NE(0, time);

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testIsDirectoryEmptyUtf8) {
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmptyUtf8(""));

    //test with an empty directory
    const std::string test_dir_name = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.tmp";;
    ASSERT_TRUE(ra::filesystem::CreateDirectoryUtf8(test_dir_name.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmptyUtf8(test_dir_name));
    
    //test with a sub directory
    const std::string sub_dir_path = test_dir_name + ra::filesystem::GetPathSeparatorStr() + "subdir.psi_\xCE\xA8_psi";
    ASSERT_TRUE(ra::filesystem::CreateDirectoryUtf8(sub_dir_path.c_str()));
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmptyUtf8(test_dir_name));
    
    //cleanup (sub directory)
    ASSERT_TRUE(ra::filesystem::DeleteDirectoryUtf8(sub_dir_path.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmptyUtf8(test_dir_name));

    //test with a file in directory
    const std::string file_path = test_dir_name + ra::filesystem::GetPathSeparatorStr() + "file.psi_\xCE\xA8_psi.tmp";
    ASSERT_TRUE(ra::filesystem::WriteTextFileUtf8(file_path, "content of the file"));
    ASSERT_FALSE(ra::filesystem::IsDirectoryEmptyUtf8(test_dir_name));
    
    //cleanup (file)
    ASSERT_TRUE(ra::filesystem::DeleteFileUtf8(file_path.c_str()));
    ASSERT_TRUE(ra::filesystem::IsDirectoryEmptyUtf8(test_dir_name));

    //cleanup (test directory)
    ra::filesystem::DeleteDirectoryUtf8(test_dir_name.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, DISABLED_testGetCurrentDirectoryUtf8) {
    std::string log_filename = ra::testing::GetTestQualifiedName() + ".log";

    const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Create a working directory that matches current test name with an utf-8 character
    std::string test_dir_name_utf8 = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi";
    std::string test_dir_path_utf8 = ra::process::GetCurrentProcessDir() + separator + test_dir_name_utf8;
    bool success = filesystem::CreateDirectoryUtf8(test_dir_path_utf8.c_str());
    ASSERT_TRUE(success);

    //clone current process executable into another process which name contains an utf8 character.
    std::string duplicate_process_filename = ra::testing::GetTestQualifiedName();
#ifdef _WIN32
    duplicate_process_filename.append(".exe");
#endif
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    std::string duplicate_process_path_utf8 = test_dir_path_utf8 + separator + duplicate_process_filename;
    bool copied = ra::filesystem::CopyFileUtf8(current_process_path, duplicate_process_path_utf8);
    ASSERT_TRUE(copied);

#ifdef _WIN32
    //The current RapidAssist does not provide a StartProcessUtf8() api yet.
    //We cannot launch a process from the command prompt if the process directory have an utf-8 character.
    //To get around this, we look for the 8.3 directory name and use this name for reaching the file.
    std::string test_dir_name_short = ra::filesystem::GetShortPathForm(test_dir_name_utf8);
    std::string test_dir_path_short = ra::process::GetCurrentProcessDir() + separator + test_dir_name_short;
    bool test_dir_path_short_found = ra::filesystem::DirectoryExists(test_dir_path_short.c_str());
    ASSERT_TRUE(test_dir_path_short_found);

    //Run the new process and log the output
    std::string command;
    command.append("cd /d \"");
    command.append(test_dir_path_short);
    command.append("\" & ");
    command.append(duplicate_process_filename);
    command.append(" --OutputGetCurrentDirectoryUtf8");
    command.append(">");
    command.append(log_filename);
    int exit_code = system(command.c_str());
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#elif defined(__linux__) || defined(__APPLE__)
    //Run the new process and log the output
    std::string command;
    command.append("cd \"");
    command.append(test_dir_path_utf8);
    command.append("\" && chmod +x ");
    command.append(duplicate_process_filename);
    command.append(" && ./");
    command.append(duplicate_process_filename);
    command.append(" --OutputGetCurrentDirectoryUtf8");
    command.append(">");
    command.append(log_filename);
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#endif //_WIN32

    //Search the log file for a valid utf-8 encoded path
    std::string log_path_utf8 = test_dir_path_utf8 + separator + log_filename;
    bool log_found = ra::filesystem::FileExistsUtf8(log_path_utf8.c_str());
    ASSERT_TRUE(log_found);

    std::string file_content;
    bool file_readed = ra::filesystem::ReadTextFileUtf8(log_path_utf8, file_content);
    ASSERT_TRUE(file_readed);

    bool found_path = (file_content.find(test_dir_path_utf8) != std::string::npos);
    ASSERT_TRUE(found_path);

    //cleanup
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testCopyFileBasicUtf8) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPathUtf8();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectoryUtf8();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".psi_\xCE\xA8_psi.tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(process_path.c_str()));
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(temp_dir.c_str()));

    bool copied = ra::filesystem::CopyFileUtf8(process_path, output_path);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSizeUtf8(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSizeUtf8(output_path.c_str());
    ASSERT_EQ(source_size, target_size);
  }
  //--------------------------------------------------------------------------------------------------
  static bool gProgressBeginUtf8 = false;
  static bool gProgressEndUtf8 = false;
  void myCopyFileCallbackFunctionUtf8(double progress) {
    //remember first and last callbacks
    if (progress == 0.0)
      gProgressBeginUtf8 = true;
    if (progress == 1.0)
      gProgressEndUtf8 = true;

    printf("%s(%.2f)\n", __FUNCTION__, progress);
  }
  TEST_F(TestFilesystemUtf8, testCopyFileCallbackFunctionUtf8) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPathUtf8();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectoryUtf8();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".omega_\xCE\xA9_omega.tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(process_path.c_str()));
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(temp_dir.c_str()));

    gProgressBeginUtf8 = false;
    gProgressEndUtf8 = false;
    bool copied = ra::filesystem::CopyFileUtf8(process_path, output_path, &myCopyFileCallbackFunctionUtf8);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSizeUtf8(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSizeUtf8(output_path.c_str());
    ASSERT_EQ(source_size, target_size);

    //assert that first and last progress was received
    ASSERT_TRUE(gProgressBeginUtf8);
    ASSERT_TRUE(gProgressEndUtf8);
  }
  //--------------------------------------------------------------------------------------------------
  class CopyFileCallbackFunctorUtf8 : public virtual ra::filesystem::IProgressReport {
  public:
    CopyFileCallbackFunctorUtf8() : progress_begin_(false), progress_end_(false) {};
    virtual ~CopyFileCallbackFunctorUtf8() {};
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
  TEST_F(TestFilesystemUtf8, testCopyFileCallbackFunctorUtf8) {
    //copy this process executable to another file
    const std::string process_path = ra::process::GetCurrentProcessPathUtf8();
    const std::string process_filename = ra::filesystem::GetFilename(process_path.c_str());
    const std::string temp_dir = filesystem::GetTemporaryDirectoryUtf8();
    const std::string output_filename = ra::testing::GetTestQualifiedName() + "." + process_filename + ".psi_\xCE\xA8_psi.tmp";
    const std::string output_path = temp_dir + ra::filesystem::GetPathSeparator() + output_filename;

    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(process_path.c_str()));
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(temp_dir.c_str()));

    CopyFileCallbackFunctorUtf8 functor;
    bool copied = ra::filesystem::CopyFileUtf8(process_path, output_path, &functor);
    ASSERT_TRUE(copied) << "Failed to copy file '" << process_path.c_str() << "' to '" << output_path.c_str() << "'.";
    ASSERT_TRUE(ra::filesystem::FileExistsUtf8(output_path.c_str())) << "File '" << output_path.c_str() << "' not found.";

    uint32_t source_size = ra::filesystem::GetFileSizeUtf8(process_path.c_str());
    uint32_t target_size = ra::filesystem::GetFileSizeUtf8(output_path.c_str());
    ASSERT_EQ(source_size, target_size);

    //assert that first and last progress was received
    ASSERT_TRUE(functor.hasProgressBegin());
    ASSERT_TRUE(functor.hasProgressEnd());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testPeekFileUtf8) {
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
    bool write_ok = ra::filesystem::WriteFileUtf8(file_path, buffer);
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
        bool peek_ok = ra::filesystem::PeekFileUtf8(file_path, peek_size, buffer);
        ASSERT_TRUE(peek_ok) << "Failed peeking " << peek_size << " bytes into file '" << file_path << "'.";
        ASSERT_EQ(peek_size, buffer.size());
      }
    }

    //peek into the file
    {
      const size_t peek_size = 155;
      bool peek_ok = ra::filesystem::PeekFileUtf8(file_path, peek_size, buffer);
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
      const uint32_t file_size = (size_t)ra::filesystem::GetFileSizeUtf8(file_path.c_str());
      const uint32_t peek_size = file_size + 10000;
      bool peek_ok = ra::filesystem::PeekFileUtf8(file_path, peek_size, buffer);
      ASSERT_TRUE(peek_ok);
      ASSERT_EQ(file_size, buffer.size());
    }

    //cleanup
    ra::filesystem::DeleteFileUtf8(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystemUtf8, testPeekFileLargeFileUtf8) {
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
      std::string filename = ra::testing::GetTestQualifiedName() + "." + user_size + ".psi_\xCE\xA8_psi.tmp";
      ra::strings::Replace(filename, SPACE, EMPTY);
      printf("Creating sparse file of size %s...\n", user_size.c_str());

      //setup cleanup in case of failures
      struct FileCleanupCallback {
        FileCleanupCallback(const char * filename) : mFilename(filename) {}
        ~FileCleanupCallback() {
          ra::filesystem::DeleteFileUtf8(mFilename);
        }
      private:
        const char * mFilename;
      } _FileCleanupCallbackInstance(filename.c_str());

      bool created = ra::testing::CreateFileSparseUtf8(filename.c_str(), expected_size);
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
        bool peek_ok = ra::filesystem::PeekFileUtf8(filename, peek_size, buffer);
        ASSERT_TRUE(peek_ok);
        ASSERT_EQ(peek_size, buffer.size());
      }

      //cleanup
      ra::filesystem::DeleteFileUtf8(filename.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace filesystem
} //namespace ra
