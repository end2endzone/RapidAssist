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

#include "TestProcessUtf8.h"
#include "rapidassist/process_utf8.h"
#include "rapidassist/environment.h"
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user.h"

#ifdef __linux__
#include <sys/wait.h>
#endif

namespace ra { namespace test
{
  extern bool CloneProcess(std::string & output_dir_path, std::string & new_process_path, const bool support_utf8, std::string & error_message);  
} //namespace test
} //namespace ra

namespace ra { namespace process { namespace test
{
  extern ProcessIdList getNewProcesses(const ProcessIdList & p1, const ProcessIdList & p2);

  //--------------------------------------------------------------------------------------------------
  void TestProcessUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestProcessUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentProcessPathUtf8) {
    static const bool support_utf8 = true;
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string test_dir_path;
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::test::CloneProcess(test_dir_path, new_process_path, support_utf8, error_message);  
    if (!cloned)
      FAIL() << error_message;

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessPathUtf8");
#ifdef _WIN32
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments[0]);
#elif __linux__
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //Search for the generated output file
    std::string expected_output_file_path = test_dir_path + separator + "SaveGetCurrentProcessPathUtf8.txt";
    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFileUtf8(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(new_process_path, content);

    //cleanup
    ra::filesystem::DeleteFileUtf8(expected_output_file_path.c_str());
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentProcessDirUtf8) {
    static const bool support_utf8 = true;
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string test_dir_path;
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::test::CloneProcess(test_dir_path, new_process_path, support_utf8, error_message);  
    if (!cloned)
      FAIL() << error_message;

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessDirUtf8");
#ifdef _WIN32
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments[0]);
#elif __linux__
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exitcode = 0;
    bool wait_ok = ra::process::WaitExit(pid, exitcode);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
    std::string expected_output_file_path = test_dir_path + separator + "SaveGetCurrentProcessDirUtf8.txt";
    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFileUtf8(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(test_dir_path, content);

    //cleanup
    ra::filesystem::DeleteFileUtf8(expected_output_file_path.c_str());
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentDirectoryUtf8) {
    static const bool support_utf8 = true;
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string test_dir_path1;
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::test::CloneProcess(test_dir_path1, new_process_path, support_utf8, error_message);  
    if (!cloned)
      FAIL() << error_message;

    //Create a temporary working directory that matches current test name and contains an utf8 character.
    std::string test_dir_name2 = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.2";
    std::string test_dir_path2 = ra::process::GetCurrentProcessDir() + separator + test_dir_name2;
    bool success = filesystem::CreateDirectoryUtf8(test_dir_path2.c_str());
    ASSERT_TRUE(success);

    //Run the new executable from test_dir_path2
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentDirectoryUtf8");
#ifdef _WIN32
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path2, arguments[0]);
#elif __linux__
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path2, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exitcode = 0;
    bool wait_ok = ra::process::WaitExit(pid, exitcode);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
    //The file is generated in the same directory as the executable.
    std::string expected_output_file_path = test_dir_path1 + separator + "SaveGetCurrentDirectoryUtf8.txt";
    ASSERT_TRUE( ra::filesystem::FileExistsUtf8(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFileUtf8(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(test_dir_path2, content);

    //cleanup
    ra::filesystem::DeleteFileUtf8(expected_output_file_path.c_str());
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path1.c_str());
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path2.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testOpenDocumentUtf8) {
    //create a text file
    const std::string newline = ra::environment::GetLineSeparator();
    const std::string content =
      ra::testing::GetTestQualifiedName() + newline +
      "The" + newline +
      "quick" + newline +
      "brown" + newline +
      "fox" + newline +
      "jumps" + newline +
      "over" + newline +
      "the" + newline +
      "lazy" + newline +
      "dog.";
    const std::string file_path = ra::process::GetCurrentProcessDir() + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.txt";
    bool success = ra::filesystem::WriteFileUtf8(file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //take a snapshot of the list of processes before opening the document
    ProcessIdList process_before = ra::process::GetProcesses();

#ifdef _WIN32
    success = ra::process::OpenDocumentUtf8(file_path);
    ASSERT_TRUE(success);
#else
    // N/A
#endif

    ra::timing::Millisleep(5000); //allow time for the process to start properly (again).

    //try to identify the new process
    ProcessIdList process_after = ra::process::GetProcesses();
    ProcessIdList new_pids = getNewProcesses(process_before, process_after);
    if (new_pids.size() == 1) {
      //found the new process that opened the document

      //kill the process
      processid_t document_pid = new_pids[0];
      bool killed = ra::process::Kill(document_pid);
      ASSERT_TRUE(killed);
    }
    else {
      //fail finding the new process
      std::string pids = ra::process::ToString(new_pids);
      printf("Warning: fail to identify the process for document '%s'\n", file_path.c_str());
      printf("Warning: the following new process may need to be closed for cleanup: %s\n", pids.c_str());
    }

    //cleanup
    ra::filesystem::DeleteFileUtf8(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
