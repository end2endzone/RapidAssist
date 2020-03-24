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
    std::string log_filename = ra::testing::GetTestQualifiedName() + ".log";

    const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Create a working directory that matches current test name
    std::string test_dir_name = ra::testing::GetTestQualifiedName();
    std::string test_dir_path = ra::process::GetCurrentProcessDir() + separator + test_dir_name;
    bool success = filesystem::CreateDirectory(test_dir_path.c_str());
    ASSERT_TRUE(success);

    //clone current process executable into another process which name contains an utf8 character.
    std::string new_process_filename = ra::testing::GetTestQualifiedName() + std::string(".psi_\xCE\xA8_psi");
#ifdef _WIN32
    new_process_filename.append(".exe");
#endif
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    std::string new_process_path_utf8 = test_dir_path + separator + new_process_filename;
    bool copied = ra::filesystem::CopyFileUtf8(current_process_path, new_process_path_utf8);
    ASSERT_TRUE(copied);

#ifdef _WIN32
    //The current RapidAssist does not provide a StartProcessUtf8() api yet.
    //We cannot launch a process from the command prompt if the process filename have an utf-8 character.
    //To get around this, we look for the 8.3 filename and use this filename for testing.
    std::string short_new_process_filename = ra::filesystem::GetShortPathForm(new_process_filename);
    std::string short_new_process_path = test_dir_path + separator + short_new_process_filename;
    bool short_full_path_found = ra::filesystem::FileExists(short_new_process_path.c_str());
    ASSERT_TRUE(short_full_path_found);

    //Run the new process and log the output
    std::string command;
    command.append("cd /d \"");
    command.append(test_dir_path);
    command.append("\" & ");
    command.append(short_new_process_filename);
    command.append(" --OutputGetCurrentProcessPathUtf8");
    command.append(">");
    command.append(log_filename);
    int exit_code = system(command.c_str());
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#elif __linux__
    //Run the new process and log the output
    std::string command;
    command.append("cd \"");
    command.append(test_dir_path);
    command.append("\" && chmod 777 ");
    command.append(new_process_filename);
    command.append(" && ./");
    command.append(new_process_filename);
    command.append(" --OutputGetCurrentProcessPathUtf8");
    command.append(">");
    command.append(log_filename);
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
#endif //_WIN32

    //Search the log file for a valid utf-8 encoded path
    std::string log_path = test_dir_path + separator + log_filename;
    bool log_found = ra::filesystem::FileExists(log_path.c_str());
    ASSERT_TRUE(log_found);

    std::string file_content;
    bool file_readed = ra::filesystem::ReadTextFile(log_path, file_content);
    ASSERT_TRUE(file_readed);

    bool found_path = (file_content.find(new_process_path_utf8) != std::string::npos);
    ASSERT_TRUE(found_path);

    //cleanup
    ra::filesystem::DeleteDirectoryUtf8(test_dir_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcessUtf8, testGetCurrentProcessDirUtf8) {
    const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Create a working directory that matches current test name and contains an utf8 character.
    std::string test_dir_name = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi";
    std::string test_dir_path = ra::process::GetCurrentProcessDir() + separator + test_dir_name;
    bool success = filesystem::CreateDirectoryUtf8(test_dir_path.c_str());
    ASSERT_TRUE(success);

    //clone current process executable into another process which name contains an utf8 character.
    std::string new_process_filename = ra::testing::GetTestQualifiedName() + std::string(".omega_\xCE\xA9_omega");
#ifdef _WIN32
    new_process_filename.append(".exe");
#endif
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    std::string new_process_path_utf8 = test_dir_path + separator + new_process_filename;
    bool copied = ra::filesystem::CopyFileUtf8(current_process_path, new_process_path_utf8);
    ASSERT_TRUE(copied);

#ifdef __linux__
    //Set new process as executable
    {
      std::string command;
      command.append("chmod 777 ");
      command.append(new_process_path_utf8);
      int system_result = system(command.c_str());
      int exit_code = WEXITSTATUS( system_result );
      ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
    }
#endif //__linux__

    //Run the new executable
#ifdef _WIN32
    std::string arguments = "--SaveGetCurrentProcessDirUtf8";
    processid_t pid = StartProcessUtf8(new_process_path_utf8, test_dir_path, arguments);
#elif __linux__
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessDirUtf8");
    processid_t pid = StartProcessUtf8(new_process_path_utf8, test_dir_path, arguments);
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
    const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Create a working directory (1) that matches current test name and contains an utf8 character.
    std::string test_dir_name1 = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.1";
    std::string test_dir_path1 = ra::process::GetCurrentProcessDir() + separator + test_dir_name1;
    bool success = filesystem::CreateDirectoryUtf8(test_dir_path1.c_str());
    ASSERT_TRUE(success);

    //Create a working directory (2) that matches current test name and contains an utf8 character.
    std::string test_dir_name2 = ra::testing::GetTestQualifiedName() + ".psi_\xCE\xA8_psi.2";
    std::string test_dir_path2 = ra::process::GetCurrentProcessDir() + separator + test_dir_name2;
    success = filesystem::CreateDirectoryUtf8(test_dir_path2.c_str());
    ASSERT_TRUE(success);

    //clone current process executable into another process which name contains an utf8 character.
    std::string new_process_filename = ra::testing::GetTestQualifiedName() + std::string(".omega_\xCE\xA9_omega");
#ifdef _WIN32
    new_process_filename.append(".exe");
#endif
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    std::string new_process_path_utf8 = test_dir_path1 + separator + new_process_filename;
    bool copied = ra::filesystem::CopyFileUtf8(current_process_path, new_process_path_utf8);
    ASSERT_TRUE(copied);

#ifdef __linux__
    //Set new process as executable
    {
      std::string command;
      command.append("chmod 777 ");
      command.append(new_process_path_utf8);
      int system_result = system(command.c_str());
      int exit_code = WEXITSTATUS( system_result );
      ASSERT_EQ(0, exit_code) << "Failed running command: " << command;
    }
#endif //__linux__

    //Run the new executable from test_dir_path2
#ifdef _WIN32
    std::string arguments = "--SaveGetCurrentDirectoryUtf8";
    processid_t pid = StartProcessUtf8(new_process_path_utf8, test_dir_path2, arguments);
#elif __linux__
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentDirectoryUtf8");
    processid_t pid = StartProcessUtf8(new_process_path_utf8, test_dir_path2, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exitcode = 0;
    bool wait_ok = ra::process::WaitExit(pid, exitcode);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
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
