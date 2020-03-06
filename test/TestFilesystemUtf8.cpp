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
#include "rapidassist/testing.h"
#include "rapidassist/process.h"
#include "rapidassist/environment.h"
#include "rapidassist/environment_utf8.h"

namespace ra { namespace filesystem { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestFilesystemUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestFilesystemUtf8::TearDown() {
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
  TEST_F(TestFilesystemUtf8, testHasReadWriteAccessUtf8) {
    std::string filename = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";

    uint32_t file_length = 365;
    std::string content = std::string(file_length, '1');

    bool file_write = ra::filesystem::WriteTextFileUtf8(filename, content);
    ASSERT_TRUE(file_write);

    bool have_read = ra::filesystem::HasReadAccessUtf8(filename.c_str());
    ASSERT_TRUE(have_read);

    bool have_write = ra::filesystem::HasWriteAccessUtf8(filename.c_str());
    ASSERT_TRUE(have_write);

#ifdef _WIN32
    //On Windows, the HasReadAccessUtf8() and HasWriteAccessUtf8() should not be working
    bool have_read_win32 = ra::filesystem::HasReadAccess(filename.c_str());
    ASSERT_FALSE(have_read_win32);
    bool have_write_win32 = ra::filesystem::HasWriteAccess(filename.c_str());
    ASSERT_FALSE(have_write_win32);
#endif

    //cleanup
    ra::filesystem::DeleteFileUtf8(filename.c_str());
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
} //namespace test
} //namespace filesystem
} //namespace ra
