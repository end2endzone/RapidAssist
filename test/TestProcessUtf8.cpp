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
} //namespace test
} //namespace process
} //namespace ra
