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
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/user.h"

#include <stdlib.h> //for system()
#ifdef __linux__
#include <sys/wait.h> //for WEXITSTATUS
#endif

namespace ra { namespace process { namespace test
{
  ProcessIdList GetNewProcesses(const ProcessIdList & p1, const ProcessIdList & p2) {
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
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path = ra::filesystem::GetParentPath(new_process_path);

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessPath");
#ifdef _WIN32
    processid_t pid = StartProcess(new_process_path, test_dir_path, arguments[0]);
#elif __linux__
    processid_t pid = StartProcess(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
    std::string expected_output_file_path = test_dir_path + separator + "SaveGetCurrentProcessPath.txt";
    ASSERT_TRUE( ra::filesystem::FileExists(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFile(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(new_process_path, content);

    //cleanup
    ra::filesystem::DeleteFile(expected_output_file_path.c_str());
    ra::filesystem::DeleteFile(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessDir) {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path = ra::filesystem::GetParentPath(new_process_path);

    //Run the new executable
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentProcessDir");
#ifdef _WIN32
    processid_t pid = StartProcess(new_process_path, test_dir_path, arguments[0]);
#elif __linux__
    processid_t pid = StartProcess(new_process_path, test_dir_path, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
    std::string expected_output_file_path = test_dir_path + separator + "SaveGetCurrentProcessDir.txt";
    ASSERT_TRUE( ra::filesystem::FileExists(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFile(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(test_dir_path, content);

    //cleanup
    ra::filesystem::DeleteFile(expected_output_file_path.c_str());
    ra::filesystem::DeleteFile(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentDirectory) {
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string test_dir_path1 = ra::filesystem::GetParentPath(new_process_path);

    //Create a temporary working directory that matches current test name and contains an utf8 character.
    std::string test_dir_name2 = ra::testing::GetTestQualifiedName() + ".2";
    std::string test_dir_path2 = ra::process::GetCurrentProcessDir() + separator + test_dir_name2;
    bool success = filesystem::CreateDirectory(test_dir_path2.c_str());
    ASSERT_TRUE(success);

    //Run the new executable from test_dir_path2
    ra::strings::StringVector arguments;
    arguments.push_back("--SaveGetCurrentDirectory");
#ifdef _WIN32
    processid_t pid = StartProcess(new_process_path, test_dir_path2, arguments[0]);
#elif __linux__
    processid_t pid = StartProcess(new_process_path, test_dir_path2, arguments);
#endif
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    ASSERT_TRUE(wait_ok);

    //Search for the generated output file
    //The file is generated in the same directory as the executable.
    std::string expected_output_file_path = test_dir_path1 + separator + "SaveGetCurrentDirectory.txt";
    ASSERT_TRUE( ra::filesystem::FileExists(expected_output_file_path.c_str()) );

    //Read the file
    std::string content;
    bool readed = ra::filesystem::ReadTextFile(expected_output_file_path, content);
    ASSERT_TRUE( readed );

    //ASSERT
    ASSERT_EQ(test_dir_path2, content);

    //cleanup
    ra::filesystem::DeleteFile(expected_output_file_path.c_str());
    ra::filesystem::DeleteFile(new_process_path.c_str());
    ra::filesystem::DeleteDirectory(test_dir_path2.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessPath2) {
    std::string path = ra::process::GetCurrentProcessPath();
    printf("Process path: %s\n", path.c_str());
    ASSERT_TRUE(!path.empty());
    ASSERT_TRUE(ra::filesystem::FileExists(path.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessDir2) {
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
    ASSERT_FALSE(ra::process::IsRunning(fake_pid)) << "The process with a process id " << fake_pid << " is running which is unexpected!";

    //expect all existing processes are running
    printf("Getting the list of active processes...\n");
    ProcessIdList processes = ra::process::GetProcesses();
    ASSERT_NE(0, processes.size());

    printf(
      "Asserting that processes from ra::process::GetProcesses() are running...\n"
      "Some process might be terminated by the time we validate the returned list of process ids.\n"
      "This is normal but it should not happend often.\n");

    //Expect that all processes retreived from GetProcesses() are all runnings
    ProcessIdList stopped_processes;
    for (size_t i = 0; i < processes.size(); i++) {
      processid_t pid = processes[i];
      if (!ra::process::IsRunning(pid))
        stopped_processes.push_back(pid);
    }
    ASSERT_LE(stopped_processes.size(), 2) << "There is " << stopped_processes.size() << " processes from the list that are not running. This is more than expected. The following process ids were not running: " << ra::process::ToString(stopped_processes);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testStartProcessWithDirectory) {
    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    std::string process_dir = ra::filesystem::GetParentPath(new_process_path);

    //define the argument 
#ifdef _WIN32
    const std::string arguments = "--SaveGetCurrentDirectory";
#else
    ra::strings::StringVector arguments;
    arguments.push_back(new_process_path);
    arguments.push_back("--SaveGetCurrentDirectory");
#endif

    static const std::string separator = ra::filesystem::GetPathSeparatorStr();
    static const std::string output_filename = "SaveGetCurrentDirectory.txt";

    //Define a list of test directories
    ra::strings::StringVector directories;
    directories.push_back(ra::filesystem::GetCurrentDirectory());     // current directory
    directories.push_back(ra::filesystem::GetTemporaryDirectory());   // temp directory
    directories.push_back(ra::user::GetHomeDirectory());              // user's home directory

    //Test process from each directory
    for(size_t i=0; i<directories.size(); i++)
    {
      const std::string & test_dir = directories[i];

      //Compute the command's expected output filename
      std::string output_file_path = process_dir + separator + output_filename;
      if (ra::filesystem::FileExists(output_file_path.c_str()))
        ra::filesystem::DeleteFile(output_file_path.c_str());

      //start the process
      ra::process::processid_t pid = ra::process::StartProcess(new_process_path, test_dir, arguments);
      ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

      //Wait for the process to exit naturally.
      ASSERT_TRUE(ra::process::WaitExit(pid));

      //Check process successful exit code.
      int exit_code = -1;
      ASSERT_TRUE(ra::process::GetExitCode(pid, exit_code));
      ASSERT_EQ(0, exit_code);

      //Assert the generated file is found
      ASSERT_TRUE(ra::filesystem::FileExists(output_file_path.c_str())) << "The expected generated file '" << output_file_path.c_str() << "' was not found.";

      //Read the content of the file
      std::string file_content;
      ASSERT_TRUE(ra::filesystem::ReadTextFile(output_file_path.c_str(), file_content));

      //The content of the file should be the process' starting directory
      ASSERT_EQ(test_dir, file_content);

      //Cleanup
      ra::filesystem::DeleteFile(output_file_path.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testTerminate) {
    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    //run the process from the current directory
    const std::string test_dir = ra::process::GetCurrentProcessDir();

    printf("Launching '%s'...\n", new_process_path.c_str());
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .
    
    //define the argument 
#ifdef _WIN32
    const std::string arguments = "--WaitForTerminateSignal";
#else
    ra::strings::StringVector arguments;
    arguments.push_back(new_process_path);
    arguments.push_back("--WaitForTerminateSignal");
#endif

    //start the process
    ra::process::processid_t pid = ra::process::StartProcess(new_process_path, test_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    printf("Created process with pid=%d\n", (int)pid);
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    ra::timing::Millisleep(3000); //allow time for the process to start properly.

    //assert the process is started
    bool started = ra::process::IsRunning(pid);
    ASSERT_TRUE(started) << "The process with pid " << pid << " does not seems to be running anymore.";

    printf("Terminating '%s' with pid=%d...\n", new_process_path.c_str(), (int)pid);
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    //try to terminate the process
    bool killed = ra::process::Terminate(pid);
    ASSERT_TRUE(killed) << "The process with pid " << pid << " was not terminated.";

    printf("Process terminated...\n");
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    //assert the process is not running anymore
    started = ra::process::IsRunning(pid);
    ASSERT_FALSE(started);

    //cleanup
    ra::filesystem::DeleteFile(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testKill) {
    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    //run the process from the current directory
    const std::string test_dir = ra::process::GetCurrentProcessDir();

    printf("Launching '%s'...\n", new_process_path.c_str());
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .
    
    //define the argument 
#ifdef _WIN32
    const std::string arguments = "--WaitForKillSignal";
#else
    ra::strings::StringVector arguments;
    arguments.push_back(new_process_path);
    arguments.push_back("--WaitForKillSignal");
#endif

    //start the process
    ra::process::processid_t pid = ra::process::StartProcess(new_process_path, test_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    printf("Created process with pid=%d\n", (int)pid);
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    ra::timing::Millisleep(3000); //allow time for the process to start properly.

    //assert the process is started
    bool started = ra::process::IsRunning(pid);
    ASSERT_TRUE(started) << "The process with pid " << pid << " does not seems to be running anymore.";

    printf("Killing '%s' with pid=%d...\n", new_process_path.c_str(), (int)pid);
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    //try to kill the process
    bool killed = ra::process::Kill(pid);
    ASSERT_TRUE(killed) << "The process with pid " << pid << " was not killed.";

    printf("Process killed...\n");
    fflush(NULL); //flush output buffer. This is required to get expected output on appveyor's .

    //assert the process is not running anymore
    started = ra::process::IsRunning(pid);
    ASSERT_FALSE(started);

    //cleanup
    ra::filesystem::DeleteFile(new_process_path.c_str());
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
    ProcessIdList new_pids = GetNewProcesses(process_before, process_after);
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
    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    //run the process from the current directory
    const std::string curr_dir = ra::process::GetCurrentProcessDir();

    printf("Launching '%s'...\n", new_process_path.c_str());
    fflush(NULL);
    
    //define the sleep x seconds command arguments
    const std::string sleep_time = "3000";
#ifdef _WIN32
    const std::string arguments = "--SleepTime=" + sleep_time;
#else
    ra::strings::StringVector arguments;
    arguments.push_back(sleep_time);
    arguments.push_back(std::string("--SleepTime=") + sleep_time);
#endif

    //start the process
    ra::process::processid_t pid = ra::process::StartProcess(new_process_path, curr_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //assert that process is started
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //wait a little to be in the middle of execution of the process
    ra::timing::Millisleep(500);

    printf("Calling ra::process::GetExitCode() while process is running...\n");
    fflush(NULL);
    int code = 0;
    bool success = ra::process::GetExitCode(pid, code);

    //assert GetExitCode fails while the process is running
    ASSERT_FALSE(success);

    printf("Call failed which is expected.\n");
    printf("Waiting for the process to exit gracefully...\n");
    fflush(NULL);

    //wait for the program to exit
    success = ra::process::WaitExit(pid);
    ASSERT_TRUE(success);

    //try again
    printf("Calling ra::process::GetExitCode() again...\n");
    success = ra::process::GetExitCode(pid, code);

    ASSERT_TRUE(success);
    ASSERT_EQ(0, code); //assert application exit code is SUCCESS.

    printf("Received expected exit code\n");

    //cleanup
    ra::filesystem::DeleteFile(new_process_path.c_str());
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
    //clone current process executable into another process.
    std::string new_process_path;
    std::string error_message;
    bool cloned = ra::testing::CloneExecutableTempFile(new_process_path, error_message);  
    ASSERT_TRUE(cloned) << error_message;

    //run the process from the current directory
    const std::string curr_dir = ra::process::GetCurrentProcessDir();

    printf("Launching '%s'...\n", new_process_path.c_str());
    fflush(NULL);
    
    //define the sleep x seconds command arguments
    const std::string sleep_time = "5000";
#ifdef _WIN32
    const std::string arguments = "--SleepTime=" + sleep_time;
#else
    ra::strings::StringVector arguments;
    arguments.push_back(sleep_time);
    arguments.push_back(std::string("--SleepTime=") + sleep_time);
#endif

    //remember which time it is
    double time_start = ra::timing::GetMillisecondsTimer();

    //start the process
    ra::process::processid_t pid = ra::process::StartProcess(new_process_path, curr_dir, arguments);
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);

    //assert that process was launched and running
    ASSERT_NE(pid, ra::process::INVALID_PROCESS_ID);
    ASSERT_TRUE(ra::process::IsRunning(pid));

    //wait for the process to complete
    printf("Waiting for the sleep process to exit...\n", new_process_path.c_str());
    fflush(NULL);
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    ASSERT_TRUE(wait_ok);

    //assert the process is not running anymore
    ASSERT_FALSE(ra::process::IsRunning(pid));

    //compute elapsed time
    double time_end = ra::timing::GetMillisecondsTimer();
    double elapsed_seconds = time_end - time_start;

    //assert elapsed time matches expected time based on the argument
    //test runtime should be bigger than sleep time
    ASSERT_GE(elapsed_seconds, 4.9);

    //cleanup
    ra::filesystem::DeleteFile(new_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
