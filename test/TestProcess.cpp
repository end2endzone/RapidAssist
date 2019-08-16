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
    ASSERT_NE(0, processes.size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessId)
  {
    processid_t curr_pid = ra::process::getCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testIsRunning)
  {
    processid_t curr_pid = ra::process::getCurrentProcessId();
    ASSERT_NE(curr_pid, ra::process::INVALID_PROCESS_ID);

    ASSERT_TRUE( ra::process::isRunning(curr_pid) );
    ASSERT_FALSE( ra::process::INVALID_PROCESS_ID );
    ASSERT_TRUE( ra::process::isRunning(curr_pid) );

    //test with a random process id
    processid_t fake_pid = 12345678;
    ASSERT_FALSE( ra::process::isRunning(fake_pid) );

    //expect all existing processes are running
    ProcessIdList processes = getProcesses();
    ASSERT_NE(0, processes.size());
    for(size_t i=0; i<processes.size(); i++)
    {
      processid_t pid = processes[i];
      ASSERT_TRUE( ra::process::isRunning(pid) );
    }
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
    // N/A
#endif

    //run the process from the current directory
    const std::string test_dir = ra::process::getCurrentProcessDir();
    ASSERT_TRUE(ra::filesystem::fileExists(exec_path.c_str()));

    //start the process
    ra::process::processid_t pid = ra::process::startProcess(exec_path, arguments, test_dir);
    ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );

    ra::time::millisleep(5000); //allow time for the process to start properly.

    //assert the process is started
    bool started = ra::process::isRunning(pid);
    ASSERT_TRUE(started);

    //try to kill the process
    bool killed = ra::process::kill(pid);
    ASSERT_TRUE(killed);

    //start the process (again)
    pid = ra::process::startProcess(exec_path, arguments, test_dir);
    ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );

    ra::time::millisleep(5000); //allow time for the process to start properly (again).

    //assert the process is started (again)
    started = ra::process::isRunning(pid);
    ASSERT_TRUE(started);

    //try to terimnate the process
    bool terminated = ra::process::terminate(pid);
    ASSERT_TRUE(terminated);

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
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
    if (new_pids.size() == 10000)
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
