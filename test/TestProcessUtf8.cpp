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
#include "rapidassist/testing_utf8.h"
#include "rapidassist/timing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user.h"

namespace ra { namespace process { namespace test
{
  extern ProcessIdList GetNewProcesses(const ProcessIdList & p1, const ProcessIdList & p2);

  //--------------------------------------------------------------------------------------------------
  void TestProcessUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestProcessUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentProcessPathUtf8) {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFileUtf8(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path = ra::filesystem::GetParentPath(new_process_path);

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessPathUtf8");
#ifdef _WIN32
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments[0]);
#elif defined(__linux__) || defined(__APPLE__)
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    ASSERT_TRUE(wait_ok);

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
    ra::filesystem::DeleteFileUtf8(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentProcessDirUtf8) {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFileUtf8(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path = ra::filesystem::GetParentPath(new_process_path);

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessDirUtf8");
#ifdef _WIN32
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments[0]);
#elif defined(__linux__) || defined(__APPLE__)
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
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
    ra::filesystem::DeleteFileUtf8(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentDirectoryUtf8) {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFileUtf8(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path1 = ra::filesystem::GetParentPath(new_process_path);

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
#elif defined(__linux__) || defined(__APPLE__)
    processid_t pid = StartProcessUtf8(new_process_path, test_dir_path2, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
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
    ra::filesystem::DeleteFile(new_process_path.c_str());
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
    ASSERT_NE(0, process_before.size());

    success = ra::process::OpenDocumentUtf8(file_path);
    ASSERT_TRUE(success);

    ra::timing::Millisleep(5000); //allow time for the process to start properly (again).

    //try to identify the new process
    ProcessIdList process_after = ra::process::GetProcesses();
    ProcessIdList new_pids = GetNewProcesses(process_before, process_after);
    if (new_pids.size() == 1) {
      //found the new process that opened the document
      printf("Found pid %s for document '%s'\n", ra::strings::ToString(new_pids[0]).c_str(), file_path.c_str());

      //kill the process
      processid_t document_pid = new_pids[0];
      bool killed = ra::process::Kill(document_pid);
      ASSERT_TRUE(killed);
    }
    else if (new_pids.size() > 1) {
      //fail finding the new process
      std::string pids = ra::process::ToString(new_pids);
      printf("Warning: failed to identify the exact process for document '%s'\n", file_path.c_str());
      printf("Warning: the following new processes may need to be closed for cleanup: %s\n", pids.c_str());
    }
    else {
      std::string msg;
      msg += ra::strings::Format("Warning: fail to identify a process for document '%s'.\n", file_path.c_str());
      msg += ra::strings::Format("Warning: %s processes are running but no new processes were identified after opening the document.\n", ra::strings::ToString(process_before.size()).c_str());
      printf("%s", file_path.c_str());
      FAIL() << msg;
    }

    //cleanup
    ra::filesystem::DeleteFileUtf8(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
