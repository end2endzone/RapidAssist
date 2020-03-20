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

#include "TestProcess.h"
#include "rapidassist/process.h"
#include "rapidassist/environment.h"
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/user.h"

#ifdef __linux__
#include <sys/wait.h>
#endif

namespace ra { namespace process { namespace test
{
  ProcessIdList getNewProcesses(const ProcessIdList & p1, const ProcessIdList & p2) {
    ProcessIdList processes;

    //try to identify the new process
    for (size_t i = 0; i < p2.size(); i++) {
      processid_t p2_pid = p2[i];

      //is this process not in p1 ?
      bool found = false;
      for (size_t j = 0; j < p1.size() && !found; j++) {
        processid_t p1_pid = p1[j];
        if (p1_pid == p2_pid)
          found = true;
      }
      if (!found) {
        //that is a new process
        processes.push_back(p2_pid);
      }
    }

    return processes;
  }

  //--------------------------------------------------------------------------------------------------
  void TestProcess::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestProcess::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessPath) {
    std::string path = ra::process::GetCurrentProcessPath();
    printf("Process path: %s\n", path.c_str());
    ASSERT_TRUE(!path.empty());
    ASSERT_TRUE(ra::filesystem::FileExists(path.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessDir) {
    std::string dir = ra::process::GetCurrentProcessDir();
    printf("Process dir: %s\n", dir.c_str());
    ASSERT_TRUE(!dir.empty());
    ASSERT_TRUE(ra::filesystem::DirectoryExists(dir.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testToString) {
    ProcessIdList pids;

    //test empty list
    {
      const std::string expected = "";
      std::string actual = ra::process::ToString(pids);
      ASSERT_EQ(expected, actual);
    }

    //test single element list
    {
      pids.push_back(12);
      const std::string expected = "12";
      std::string actual = ra::process::ToString(pids);
      ASSERT_EQ(expected, actual);
    }

    //test 2 elements list
    {
      pids.push_back(34);
      const std::string expected = "12, 34";
      std::string actual = ra::process::ToString(pids);
      ASSERT_EQ(expected, actual);
    }

    //test 3 elements list
    {
      pids.push_back(56);
      const std::string expected = "12, 34, 56";
      std::string actual = ra::process::ToString(pids);
      ASSERT_EQ(expected, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetProcesses) {
    ProcessIdList processes = GetProcesses();
    size_t num_process = processes.size();
    printf("Found %s running processes\n", ra::strings::ToString(num_process).c_str());
    ASSERT_NE(0, processes.size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessId) {
    processid_t curr_pid = ra::process::GetCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testInvalidProcessId) {
    printf("ra::process::INVALID_PROCESS_ID defined as 0x%X or %s\n", ra::process::INVALID_PROCESS_ID, ra::strings::ToString(ra::process::INVALID_PROCESS_ID).c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testIsRunning) {
    processid_t curr_pid = ra::process::GetCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);

    ASSERT_FALSE(ra::process::IsRunning(ra::process::INVALID_PROCESS_ID));
    ASSERT_TRUE(ra::process::IsRunning(curr_pid));

    //test with a random process id
    processid_t fake_pid = 12345678;
    ASSERT_FALSE(ra::process::IsRunning(fake_pid));

    //expect all existing processes are running
    printf("Getting the list of active processes...\n");
    ProcessIdList processes = GetProcesses();
    ASSERT_NE(0, processes.size());

    printf("Note:\n"
      "Asserting that received processes are running...\n"
      "Some process might be terminated by the time we validate the returned list of process ids.\n"
      "This is normal but it should not happend often.\n");

    for (size_t i = 0; i < processes.size(); i++) {
      processid_t pid = processes[i];
      EXPECT_TRUE(ra::process::IsRunning(pid)) << "The process with pid " << pid << " does not appears to be running.";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testStartProcessWithDirectory) {
    printf("Note:\n"
      "This test must be manually validated.\n"
      "The test runs the same executable twice but from different directories.\n"
      "The output from the executable is expected to be different since it is run from different locations.\n");
    printf("\n");
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    //keep the current directory to verify if it has not changed
    const std::string curr_dir1 = ra::filesystem::GetCurrentDirectory();

    //will run the process from user's home directory
    const std::string home_dir = ra::user::GetHomeDirectory();
    ASSERT_TRUE(ra::filesystem::DirectoryExists(home_dir.c_str()));

    //will also run the process from a custom directory
    const std::string custom_dir = curr_dir1 + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName() + ".dir";
    bool created = ra::filesystem::CreateDirectory(custom_dir.c_str());
    ASSERT_TRUE(created);

    //create a text file in user's home directory
    const std::string newline = ra::environment::GetLineSeparator();
    const std::string content = ra::testing::GetTestQualifiedName();
    const std::string home_file_path = home_dir + ra::filesystem::GetPathSeparatorStr() + "This_file_is_in_home_directory.txt";
    bool success = ra::filesystem::WriteFile(home_file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //create a text file in custom directory
    const std::string custom_file_path = custom_dir + ra::filesystem::GetPathSeparatorStr() + "This_file_is_in_a_custom_directory.txt";
    success = ra::filesystem::WriteFile(custom_file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //define a command that lists the files in the current directory
#ifdef _WIN32
    const std::string exec_path = ra::environment::GetEnvironmentVariable("ComSpec");
    const std::string arguments = "/c dir";
#else
    ra::strings::StringVector arguments;
    const std::string exec_path = "/bin/ls";
#endif

    //build a list of directory to launch exec_path
    ra::strings::StringVector dirs;
    dirs.push_back(home_dir);
    dirs.push_back(custom_dir);

    //for each directory
    for (size_t i = 0; i < dirs.size(); i++) {
      const std::string & mydir = dirs[i];
      printf("Launching process '%s' from directory '%s':\n", exec_path.c_str(), mydir.c_str());
      printf("{\n");
      fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

      //start the process
      ra::process::processid_t pid = ra::process::StartProcess(exec_path, mydir, arguments);
      ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

      //wait for the process to complete
      int exitcode = 0;
      bool wait_ok = ra::process::WaitExit(pid, exitcode);
      ASSERT_TRUE(wait_ok);

      fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .
      printf("}\n");
      printf("\n");
      fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .
    }

    //assert that current directory is not affected by the launched processes 
    const std::string curr_dir2 = ra::filesystem::GetCurrentDirectory();
    ASSERT_EQ(curr_dir1, curr_dir2);

    //cleanup
    ra::filesystem::DeleteFile(home_file_path.c_str());
    ra::filesystem::DeleteDirectory(custom_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
#ifndef _WIN32
  void resetconsolestate() {
    //after killing nano, the console may be in a weird configuration.
    //reset the console in a "sane" configuration.
    //https://unix.stackexchange.com/questions/492809/my-bash-shell-doesnt-start-a-new-line-upon-return-and-doesnt-show-typed-comma
    //https://unix.stackexchange.com/questions/58951/accidental-nano-somefile-uniq-renders-the-shell-unresponsive
    //Note:
    //  use '/bin/stty' instead of '/usr/bin/reset' because reset will actually erase the content of the console
    //  and we do not want to loose the the previous test results and details.

    ra::strings::StringVector args;
    args.push_back("sane");
    const std::string curr_dir = ra::filesystem::GetCurrentDirectory();
    ra::process::processid_t pid = ra::process::StartProcess("/bin/stty", curr_dir, args);

    //wait for the process to complete
    int exitcode = 0;
    bool wait_ok = ra::process::WaitExit(pid, exitcode);

    printf("\n");
    fflush(NULL);
  }
  void deletenanocache(const std::string & file_path) {
    //for a filename named        "TestProcess.testProcesses.txt",
    //nano's cache file is named ".TestProcess.testProcesses.txt.swp".

    std::string parent_dir, filename;
    ra::filesystem::SplitPath(file_path, parent_dir, filename);

    const std::string cache_path = parent_dir + ra::filesystem::GetPathSeparatorStr() + "." + filename + ".swp";
    ra::filesystem::DeleteFile(cache_path.c_str());
  }
#endif
  TEST_F(TestProcess, testKillAndTerminate) {
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
    const std::string file_path = ra::process::GetCurrentProcessDir() + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName() + ".txt";
    bool success = ra::filesystem::WriteFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //define the command 
#ifdef _WIN32
    const std::string arguments = "\"" + file_path + "\"";
    const std::string exec_path = "c:\\windows\\notepad.exe";
#else
    ra::strings::StringVector arguments;
    arguments.push_back(file_path);
    const std::string exec_path = "/bin/nano";
#endif

    //run the process from the current directory
    const std::string test_dir = ra::process::GetCurrentProcessDir();
    ASSERT_TRUE(ra::filesystem::FileExists(exec_path.c_str()));

#ifndef _WIN32
    //delete nano's cache before launching nano
    deletenanocache(file_path);
#endif

    printf("Launching '%s'...\n", exec_path.c_str());

    //start the process
    ra::process::processid_t pid = ra::process::StartProcess(exec_path, test_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    ra::timing::Millisleep(5000); //allow time for the process to start properly.

    //assert the process is started
    bool started = ra::process::IsRunning(pid);
    ASSERT_TRUE(started);

    printf("Killing '%s' with pid=%d...\n", exec_path.c_str(), (int)pid);

    //try to kill the process
    bool killed = ra::process::Kill(pid);
    ASSERT_TRUE(killed);

#ifndef _WIN32
    resetconsolestate();
#endif

    printf("Killed...\n");

    //assert the process is not found
    started = ra::process::IsRunning(pid);
    ASSERT_FALSE(started);

#ifndef _WIN32
    //delete nano's cache before launching nano (again)
    deletenanocache(file_path);
#endif

    printf("Launching '%s' (again)...\n", exec_path.c_str());

    //start the process (again)
    pid = ra::process::StartProcess(exec_path, test_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    ra::timing::Millisleep(5000); //allow time for the process to start properly (again).

    //assert the process is started (again)
    started = ra::process::IsRunning(pid);
    ASSERT_TRUE(started);

    printf("Terminating '%s' with pid=%d...\n", exec_path.c_str(), (int)pid);

    //try to terimnate the process
    bool terminated = ra::process::Terminate(pid);
    ASSERT_TRUE(terminated);

#ifndef _WIN32
    //delete nano's cache (cleanup)
    deletenanocache(file_path);
#endif

    printf("Terminated...\n");

    //assert the process is not found
    started = ra::process::IsRunning(pid);
    ASSERT_FALSE(started);

    //cleanup
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testOpenDocument) {
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
    const std::string file_path = ra::process::GetCurrentProcessDir() + ra::filesystem::GetPathSeparatorStr() + ra::testing::GetTestQualifiedName() + ".txt";
    bool success = ra::filesystem::WriteFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE(success);

    //take a snapshot of the list of processes before opening the document
    ProcessIdList process_before = ra::process::GetProcesses();

#ifdef _WIN32
    success = ra::process::OpenDocument(file_path);
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
    ra::filesystem::DeleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetExitCode) {
    //define the sleep x seconds command
    const std::string sleep_time = "2";
#ifdef _WIN32
    const std::string exec_path = ra::filesystem::FindFileFromPaths("sleep.exe");
    const std::string arguments = sleep_time;
#else
    ra::strings::StringVector arguments;
    arguments.push_back(sleep_time);
    const std::string exec_path = "/bin/sleep";
#endif

    //assert that given process exists
    ASSERT_TRUE(ra::filesystem::FileExists(exec_path.c_str()));

    //start the process
    const std::string curr_dir = ra::process::GetCurrentProcessDir();
    ra::process::processid_t pid = ra::process::StartProcess(exec_path, curr_dir, arguments);

    //assert that process is started
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait a little to be in the middle of execution of the process
    ra::timing::Millisleep(500);

    printf("calling ra::process::GetExitCode() while process is running...\n");
    int code = 0;
    bool success = ra::process::GetExitCode(pid, code);

    //assert GetExitCode fails while the process is running
    ASSERT_FALSE(success);

    printf("call failed which is expected.\n");
    printf("waiting for the process to exit gracefully...\n");

    //wait for the program to exit
    success = ra::process::WaitExit(pid);
    ASSERT_TRUE(success);

    //try again
    printf("calling ra::process::GetExitCode() again...\n");
    success = ra::process::GetExitCode(pid, code);

    ASSERT_TRUE(success);
    ASSERT_EQ(0, code); //assert application exit code is SUCCESS.

    printf("received expected exit code\n");
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetExitCodeSpecific) {
    //define the process exit with error code command
#ifdef _WIN32
    const int expected_error_code = 123456;
    const std::string expected_error_code_str = ra::strings::ToString(expected_error_code);
    const std::string exec_path = ra::environment::GetEnvironmentVariable("ComSpec");
    const std::string arguments = "/c exit " + expected_error_code_str;
#else
    const int expected_error_code = 234;
    const std::string expected_error_code_str = ra::strings::ToString(expected_error_code);
    ra::strings::StringVector arguments;
    arguments.push_back("-c");
    std::string exit_arg = "exit ";
    exit_arg.append(expected_error_code_str);
    arguments.push_back(exit_arg);
    const std::string exec_path = "/bin/bash";
#endif

    //assert that given process exists
    ASSERT_TRUE(ra::filesystem::FileExists(exec_path.c_str()));

    //start the process
    const std::string curr_dir = ra::process::GetCurrentProcessDir();
    ra::process::processid_t pid = ra::process::StartProcess(exec_path, curr_dir, arguments);

    //wait for the program to exit
    bool success = ra::process::WaitExit(pid);
    ASSERT_TRUE(success);

    //get the exit code
    int actual_exit_code = 0;
    success = ra::process::GetExitCode(pid, actual_exit_code);
    ASSERT_TRUE(success);
    ASSERT_EQ(expected_error_code, actual_exit_code);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testWaitExit) {
    //define the sleep x seconds command
    const std::string sleep_time = "5";
#ifdef _WIN32
    const std::string exec_path = ra::filesystem::FindFileFromPaths("sleep.exe");
    const std::string arguments = sleep_time;
#else
    ra::strings::StringVector arguments;
    arguments.push_back(sleep_time);
    const std::string exec_path = "/bin/sleep";
#endif

    //assert that given process exists
    ASSERT_TRUE(ra::filesystem::FileExists(exec_path.c_str()));

    //remember which time it is
    double time_start = ra::timing::GetMillisecondsTimer();

    //start the process
    const std::string curr_dir = ra::process::GetCurrentProcessDir();
    ra::process::processid_t pid = ra::process::StartProcess(exec_path, curr_dir, arguments);

    //assert that process was launched
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exitcode = 0;
    bool wait_ok = ra::process::WaitExit(pid, exitcode);
    ASSERT_TRUE(wait_ok);

    //compute elapsed time
    double time_end = ra::timing::GetMillisecondsTimer();
    double elapsed_seconds = time_end - time_start;

    //assert elapsed time matches expected time based on the argument
    //test runtime should be bigger than sleep time
    ASSERT_GE(elapsed_seconds, 4.9);
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
