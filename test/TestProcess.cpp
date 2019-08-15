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
#include "rapidassist/filesystem.h"

namespace ra { namespace process { namespace test
{

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
  TEST_F(TestProcess, testStartProcess)
  {
    //create a text file
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
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

#ifdef _WIN32
    const std::string arguments = "\"" + file_path + "\"";
    const std::string exec_path  = "c:\\windows\\notepad.exe";
    const std::string test_dir = ra::process::getCurrentProcessDir();
    ASSERT_TRUE(ra::filesystem::fileExists(exec_path.c_str()));

    ra::process::processid_t pid = ra::process::startProcess(exec_path, arguments, test_dir);
    ASSERT_NE( pid, ra::process::INVALID_PROCESS_ID );
#else
    // N/A
#endif

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testOpenDocument)
  {
    //create a text file
    const std::string newline = ra::environment::getLineSeparator();
    const std::string content = 
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

#ifdef _WIN32
    success = ra::process::openDocument(file_path);
    ASSERT_TRUE( success );
#else
    // N/A
#endif

    //cleanup
    ra::filesystem::deleteFile(file_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra
