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

#include "TestTesting.h"
#include "rapidassist/testing.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

namespace ra { namespace test {

  void TestTesting::SetUp() {
    ASSERT_TRUE(ra::testing::CreateFile("text1.tmp"));
    ASSERT_TRUE(ra::testing::CreateFile("text2.tmp"));
    ASSERT_TRUE(ra::testing::CreateFile("binary1.tmp", 1024));
  }

  void TestTesting::TearDown() {
    //cleanup
    ra::filesystem::DeleteFile("text1.tmp");
    ra::filesystem::DeleteFile("text2.tmp");
    ra::filesystem::DeleteFile("binary1.tmp");
  }

  TEST_F(TestTesting, testFile1NotFound) {
    std::string msg;
    const char * file1 = "notfound.tmp";
    const char * file2 = "text1.tmp";
    bool equals = ra::testing::IsFileEquals(file1, file2, msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is not found."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestTesting, testFile2NotFound) {
    std::string msg;
    const char * file1 = "text1.tmp";
    const char * file2 = "notfound.tmp";
    bool equals = ra::testing::IsFileEquals(file1, file2, msg);
    ASSERT_FALSE(equals) << msg.c_str();
    SCOPED_TRACE(msg);
    ASSERT_NE(msg.find("Second file is not found."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestTesting, testFile1Smaller) {
    std::string msg;
    const std::string file1 = ra::testing::GetTestQualifiedName() + ".1.tmp";
    const std::string file2 = ra::testing::GetTestQualifiedName() + ".2.tmp";

    ASSERT_TRUE(ra::testing::CreateFile(file1.c_str(), 1000));
    ASSERT_TRUE(ra::testing::CreateFile(file2.c_str(), 1200));

    bool equals = ra::testing::IsFileEquals(file1.c_str(), file2.c_str(), msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is smaller than Second file"), std::string::npos) << msg.c_str();

    //cleanup
    ra::filesystem::DeleteFile(file1.c_str());
    ra::filesystem::DeleteFile(file2.c_str());
  }

  TEST_F(TestTesting, testFile1Bigger) {
    std::string msg;
    const std::string file1 = ra::testing::GetTestQualifiedName() + ".1.tmp";
    const std::string file2 = ra::testing::GetTestQualifiedName() + ".2.tmp";

    ASSERT_TRUE(ra::testing::CreateFile(file1.c_str(), 1200));
    ASSERT_TRUE(ra::testing::CreateFile(file2.c_str(), 1000));

    bool equals = ra::testing::IsFileEquals(file1.c_str(), file2.c_str(), msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is bigger than Second file"), std::string::npos) << msg.c_str();

    //cleanup
    ra::filesystem::DeleteFile(file1.c_str());
    ra::filesystem::DeleteFile(file2.c_str());
  }

  TEST_F(TestTesting, testBigFileEquals) {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::testing::CreateFile(file1, 103000);
    ra::testing::CreateFile(file2, 103000);
    bool equals = ra::testing::IsFileEquals(file1, file2, msg);
    ASSERT_TRUE(equals) << msg.c_str();
    ASSERT_EQ(msg, "") << msg.c_str();

    //cleanup
    ra::filesystem::DeleteFile(file1);
    ra::filesystem::DeleteFile(file2);
  }

  TEST_F(TestTesting, testSmallFileEquals) {
    std::string msg;
    const char * file1 = "text1.tmp";
    const char * file2 = "text2.tmp";
    bool equals = ra::testing::IsFileEquals(file1, file2, msg);
    SCOPED_TRACE(msg);
    ASSERT_TRUE(equals) << msg.c_str();
    ASSERT_EQ(msg, "") << msg.c_str();
  }

  TEST_F(TestTesting, testFileDiff3) {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::testing::CreateFile(file1, 10300);
    ra::testing::CreateFile(file2, 10300);
    ra::testing::ChangeFileContent(file1, 10, 0x01);
    ra::testing::ChangeFileContent(file1, 100, 0x10);
    ra::testing::ChangeFileContent(file1, 1027, 0xaa);
    ra::testing::ChangeFileContent(file1, 10270, 0xaa);
    bool equals = ra::testing::IsFileEquals(file1, file2, msg, 3);
    ASSERT_FALSE(equals) << msg.c_str();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d(0x%X)", 10, 10);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 100, 100);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 1027, 1027);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert 4th error not specified
    sprintf(buffer, "%d(0x%X)", 10270, 10270);
    ASSERT_EQ(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert more than 3 errors found
    ASSERT_NE(msg.find("..."), std::string::npos) << msg.c_str();

    //cleanup
    ra::filesystem::DeleteFile(file1);
    ra::filesystem::DeleteFile(file2);
  }

  TEST_F(TestTesting, testFileDiffAll) {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::testing::CreateFile(file1, 10300);
    ra::testing::CreateFile(file2, 10300);
    ra::testing::ChangeFileContent(file1, 10, 0x01);
    ra::testing::ChangeFileContent(file1, 100, 0x10);
    ra::testing::ChangeFileContent(file1, 1027, 0xaa);
    ra::testing::ChangeFileContent(file1, 10270, 0xaa);
    bool equals = ra::testing::IsFileEquals(file1, file2, msg, 999);
    ASSERT_FALSE(equals) << msg.c_str();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d(0x%X)", 10, 10);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 100, 100);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 1027, 1027);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 10270, 10270);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert not more than 4 errors found
    ASSERT_EQ(msg.find("..."), std::string::npos) << msg.c_str();

    //cleanup
    ra::filesystem::DeleteFile(file1);
    ra::filesystem::DeleteFile(file2);
  }

  TEST_F(TestTesting, testCloneExecutableFileInvalidPath) {
    //Build a temporary file path
    std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string filename = ra::testing::GetTestQualifiedName();
    std::string file_path = temp_dir + path_separator + filename;

    std::string invalid_path = file_path + path_separator + "foo" + path_separator + "bar" + path_separator + "invalid";
#ifdef _WIN32
    invalid_path.append(".exe");  //Executables ends with the .exe file extension
#endif

    std::string error_message;
    bool process_file_cloned = ra::testing::CloneExecutableFile(invalid_path, error_message);
    
    ASSERT_FALSE(process_file_cloned) << "error_message=" << error_message;
    ASSERT_FALSE(error_message.empty());
  }

  TEST_F(TestTesting, testCloneExecutableFile) {
    //Build a temporary file path
    std::string temp_dir = ra::filesystem::GetTemporaryDirectory();
    std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string filename = ra::testing::GetTestQualifiedName();
    std::string file_path = temp_dir + path_separator + filename;

#ifdef _WIN32
    //Executables ends with the .exe file extension
    file_path.append(".exe");
#endif

    std::string error_message;
    bool process_file_cloned = ra::testing::CloneExecutableFile(file_path, error_message);
    ASSERT_TRUE(process_file_cloned) << "error_message=" << error_message;
    ASSERT_TRUE(error_message.empty());

    //Try to execute the copied file.
    printf("Running the cloned executable which should output the path of the copied process on the console...\n");
    std::string command;
    command.append(file_path);
    command.append(" ");
    command.append(" --OutputGetCurrentProcessPath");
#ifdef _WIN32
    int exit_code = system(command.c_str());
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#elif defined(__linux__) || defined(__APPLE__)
    //Run the new process and log the output
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#endif //_WIN32

    //Cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }

  TEST_F(TestTesting, testCloneExecutableTempFile) {
    std::string file_path;
    std::string error_message;
    bool process_file_cloned = ra::testing::CloneExecutableTempFile(file_path, error_message);
    ASSERT_TRUE(process_file_cloned) << "error_message=" << error_message;
    ASSERT_TRUE(!file_path.empty());
    ASSERT_TRUE(error_message.empty());

    //Try to execute the copied file.
    printf("Running the cloned executable which should output the path of the copied process on the console...\n");
    std::string command;
    command.append(file_path);
    command.append(" ");
    command.append(" --OutputGetCurrentProcessPath");
#ifdef _WIN32
    int exit_code = system(command.c_str());
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#elif defined(__linux__) || defined(__APPLE__)
    //Run the new process and log the output
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#endif //_WIN32

    //Cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }

  TEST_F(TestTesting, testGetTestList) {
    //Get path on the executable
    std::string process_path = ra::process::GetCurrentProcessPath();
    ASSERT_TRUE(!process_path.empty());
    ASSERT_TRUE(ra::filesystem::FileExists(process_path.c_str()));

    //Get the names of all tests supported
    ra::strings::StringVector names = ra::testing::GetTestList(process_path.c_str());
    ASSERT_TRUE(!names.empty());

    //Search for this test name in the names
    const std::string this_test = ra::testing::GetTestQualifiedName();
    bool found = false;
    for(size_t i=0; i<names.size() && !found; i++)
    {
      const std::string & name = names[i];
      if (name == this_test)
        found = true;
    }
    ASSERT_TRUE(found) << "The test '" << this_test << "' is not found in the list of tests: " << ra::strings::Join(names, "\n");
  }

} //namespace test
} //namespace ra
