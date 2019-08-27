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
#include "rapidassist/gtesthelp.h"
#include "rapidassist/time_.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/user.h"

namespace ra { namespace process { namespace test
{
  ProcessIdList getNewProcesses(const ProcessIdList & p1, const ProcessIdList & p2)
  {
    ProcessIdList processes;

    //try to identify the new process
    for(size_t i=0; i<p2.size(); i++)
    {
      processid_t p2_pid = p2[i];

      //is this process not in p1 ?
      bool found = false;
      for(size_t j=0; j<p1.size() && !found; j++)
      {
        processid_t p1_pid = p1[j];
        if (p1_pid == p2_pid)
          found = true;
      }
      if (!found)
      {
        //that is a new process
        processes.push_back(p2_pid);
      }
    }

    return processes;
  }

  //--------------------------------------------------------------------------------------------------
  void TestProcess::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestProcess::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessPath)
  {
    std::string path = ra::process::getCurrentProcessPath();
    printf("Process path: %s\n", path.c_str());
    ASSERT_TRUE(!path.empty());
    ASSERT_TRUE( ra::filesystem::fileExists(path.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessDir)
  {
    std::string dir = ra::process::getCurrentProcessDir();
    printf("Process dir: %s\n", dir.c_str());
    ASSERT_TRUE(!dir.empty());
    ASSERT_TRUE( ra::filesystem::folderExists(dir.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testToString)
  {
    ProcessIdList pids;

    //test empty list
    {
      const std::string expected = "";
      std::string actual = ra::process::toString(pids);
      ASSERT_EQ( expected, actual );
    }

    //test single element list
    {
      pids.push_back(12);
      const std::string expected = "12";
      std::string actual = ra::process::toString(pids);
      ASSERT_EQ( expected, actual );
    }

    //test 2 elements list
    {
      pids.push_back(34);
      const std::string expected = "12, 34";
      std::string actual = ra::process::toString(pids);
      ASSERT_EQ( expected, actual );
    }

    //test 3 elements list
    {
      pids.push_back(56);
      const std::string expected = "12, 34, 56";
      std::string actual = ra::process::toString(pids);
      ASSERT_EQ( expected, actual );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetProcesses)
  {
    ProcessIdList processes = getProcesses();
    size_t num_process = processes.size();
    printf("Found %s running processes\n", ra::strings::toString(num_process).c_str());
    ASSERT_NE(0, processes.size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessId)
  {
    processid_t curr_pid = ra::process::getCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testProcessIds)
  {
    printf("ra::process::INVALID_PROCESS_ID defined as 0x%X or %s\n", ra::process::INVALID_PROCESS_ID, ra::strings::toString(ra::process::INVALID_PROCESS_ID).c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testIsRunning)
  {
    processid_t curr_pid = ra::process::getCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);

    ASSERT_FALSE( ra::process::isRunning(ra::process::INVALID_PROCESS_ID) );
    ASSERT_TRUE ( ra::process::isRunning(curr_pid) );

    //test with a random process id
    processid_t fake_pid = 12345678;
    ASSERT_FALSE( ra::process::isRunning(fake_pid) );
    
    //expect all existing processes are running
    ProcessIdList processes = getProcesses();
    ASSERT_NE(0, processes.size());
    for(size_t i=0; i<processes.size(); i++)
    {
      processid_t pid = processes[i];
      EXPECT_TRUE( ra::process::isRunning(pid) ) << "The process with pid " << pid << " does not appears to be running.";
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testStartProcessWithDirectory)
  {
    printf( "Note: this test must be manually validated.\n"
            "The test runs the same excutable twice but from a different directories.\n"
            "The output from the executable should be different since it is run from different locations.\n");
    printf("\n");

    //keep the current directory to verify if it has not changed
    const std::string curr_dir1 = ra::filesystem::getCurrentFolder();

    //will run the process from user's home directory
    const std::string home_dir = ra::user::getHomeDirectory();
    ASSERT_TRUE(ra::filesystem::folderExists(home_dir.c_str()));

    //will also run the process from a custom directory
    const std::string custom_dir = curr_dir1 + ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".dir";
    bool created = ra::filesystem::createFolder(custom_dir.c_str());
    ASSERT_TRUE(created);

    //create a text file in user's home directory
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = ra::gtesthelp::getTestQualifiedName();
    const std::string home_file_path = home_dir + ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".txt";
    bool success = ra::filesystem::writeFile(home_file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //create a text file in custom directory
    const std::string custom_file_path = custom_dir + ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".txt";
    success = ra::filesystem::writeFile(custom_file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //define a command that lists the files in the current directory
#ifdef _WIN32
    const std::string exec_path  = ra::environment::getEnvironmentVariable("ComSpec");
    const std::string arguments = "/c dir /w";
#else
    ra::strings::StringVector arguments;
    const std::string exec_path  = "/bin/ls";
#endif

    //build a list of directory to launch exec_path
    ra::strings::StringVector dirs;
    dirs.push_back(home_dir);
    dirs.push_back(custom_dir);
    
    //for each directory
    for(size_t i=0; i<dirs.size(); i++)
    {
      const std::string & mydir = dirs[i];
      printf("Launching process '%s' from directory '%s':\n", exec_path.c_str(), mydir.c_str());
      printf("{\n");

      //start the process
      ra::process::processid_t pid = ra::process::startProcess(exec_path, mydir, true, arguments);
      ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );

      printf("}\n");
      printf("\n");
    }

    //assert that current directory is not affected by the launched processes 
    const std::string curr_dir2 = ra::filesystem::getCurrentFolder();
    ASSERT_EQ( curr_dir1, curr_dir2 );

    //cleanup
    ra::filesystem::deleteFile(home_file_path.c_str());
    ra::filesystem::deleteFolder(custom_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testProcesses)
  {
    //create a text file
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
      ra::gtesthelp::getTestQualifiedName() + newline +
      "The"   + newline +
      "quick" + newline +
      "brown" + newline +
      "fox"   + newline +
      "jumps" + newline +
      "over"  + newline +
      "the"   + newline +
      "lazy"  + newline +
      "dog." ;
    const std::string file_path = ra::process::getCurrentProcessDir() + ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".txt";
    bool success = ra::filesystem::writeFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //define the command 
#ifdef _WIN32
    const std::string arguments = "\"" + file_path + "\"";
    const std::string exec_path  = "c:\\windows\\notepad.exe";
#else
    ra::strings::StringVector arguments;
    arguments.push_back(file_path);
    const std::string exec_path  = "/bin/nano";
#endif

    //run the process from the current directory
    const std::string test_dir = ra::process::getCurrentProcessDir();
    ASSERT_TRUE(ra::filesystem::fileExists(exec_path.c_str()));

    //delete nano's cache file first
    //nano's cache file is named ".TestProcess.testProcesses.txt.swp"
    const std::string cache_path = test_dir + ra::filesystem::getPathSeparatorStr() + "." + ra::gtesthelp::getTestQualifiedName() + ".txt.swp";
    ra::filesystem::deleteFile(cache_path.c_str());

    //start the process
    ra::process::processid_t pid = ra::process::startProcess(exec_path, test_dir, false, arguments);
    ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );

    ra::time::millisleep(5000); //allow time for the process to start properly.
    
    //assert the process is started
    bool started = ra::process::isRunning(pid);
    ASSERT_TRUE(started);
    
    //try to kill the process
    bool killed = ra::process::kill(pid);
    ASSERT_TRUE(killed);
    
#ifndef _WIN32
    //after killing nano, the console may be in a weird configuration.
    //reset the console in a "sane" configuration.
    //https://unix.stackexchange.com/questions/492809/my-bash-shell-doesnt-start-a-new-line-upon-return-and-doesnt-show-typed-comma
    //https://unix.stackexchange.com/questions/58951/accidental-nano-somefile-uniq-renders-the-shell-unresponsive
    //Note:
    //  use '/bin/stty' instead of '/usr/bin/reset' because reset will actually erase the content of the console
    //  and we do not want to loose the the previous test results and details.
    {
      ra::strings::StringVector reset_args;
      reset_args.push_back("sane");
      ra::process::processid_t reset_pid = ra::process::startProcess("/bin/stty", test_dir, false, reset_args);
      
      //wait for the process to exit
      while (ra::process::isRunning(reset_pid))
      {
        //wait a little more
        ra::time::millisleep(1000);
      }
      
      printf("\n");
      fflush(NULL);
    }
#endif
    
    //  //start the process (again)
    //  pid = ra::process::startProcess(exec_path, test_dir, false, arguments);
    //  ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );
    //  
    //  ra::time::millisleep(5000); //allow time for the process to start properly (again).
    //  
    //  //assert the process is started (again)
    //  started = ra::process::isRunning(pid);
    //  ASSERT_TRUE(started);
    //  
    //  //try to terimnate the process
    //  bool terminated = ra::process::terminate(pid);
    //  ASSERT_TRUE(terminated);

    //cleanup
    //ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testOpenDocument)
  {
    //create a text file
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
      ra::gtesthelp::getTestQualifiedName() + newline +
      "The"   + newline +
      "quick" + newline +
      "brown" + newline +
      "fox"   + newline +
      "jumps" + newline +
      "over"  + newline +
      "the"   + newline +
      "lazy"  + newline +
      "dog." ;
    const std::string file_path = ra::process::getCurrentProcessDir() + ra::filesystem::getPathSeparatorStr() + ra::gtesthelp::getTestQualifiedName() + ".txt";
    bool success = ra::filesystem::writeFile(file_path, content); //write the file as a binary file
    ASSERT_TRUE( success );

    //take a snapshot of the list of processes before opening the document
    ProcessIdList process_before = ra::process::getProcesses();

#ifdef _WIN32
    success = ra::process::openDocument(file_path);
    ASSERT_TRUE( success );
#else
    // N/A
#endif

    ra::time::millisleep(5000); //allow time for the process to start properly (again).

    //try to identify the new process
    ProcessIdList process_after = ra::process::getProcesses();
    ProcessIdList new_pids = getNewProcesses(process_before, process_after);
    if (new_pids.size() == 1)
    {
      //found the new process that opened the document

      //kill the process
      processid_t document_pid = new_pids[0];
      bool killed = ra::process::kill(document_pid);
      ASSERT_TRUE(killed);
    }
    else
    {
      //fail finding the new process
      std::string pids = ra::process::toString(new_pids);
      printf("Warning: fail to identify the process for document '%s'\n", file_path.c_str());
      printf("Warning: the following new process may need to be closed for cleanup: %s\n", pids.c_str());
    }

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
