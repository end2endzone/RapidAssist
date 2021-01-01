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

#include <stdio.h>
#include <iostream>
#include <stdio.h>

#include <gtest/gtest.h>

#include "rapidassist/cli.h"
#include "rapidassist/testing.h"
#include "rapidassist/environment.h"

#include "CommandLineMgr.h"

int main(int argc, char **argv) {
  std::string tmp;
  bool found = false;

  //validate --OutputGetCurrentProcessPathUtf8
  found = ra::cli::ParseArgument("OutputGetCurrentProcessPathUtf8", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentProcessPathUtf8();
    return 0;
  }

  //validate --OutputGetCurrentDirectoryUtf8
  found = ra::cli::ParseArgument("OutputGetCurrentDirectoryUtf8", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentDirectoryUtf8();
    return 0;
  }

  //validate --OutputGetCurrentDirectoryUtf8
  found = ra::cli::ParseArgument("OutputGetCurrentDirectoryUtf8", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentDirectoryUtf8();
    return 0;
  }

  //validate --OutputGetCurrentProcessPath
  found = ra::cli::ParseArgument("OutputGetCurrentProcessPath", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentProcessPath();
    return 0;
  }

  //validate --OutputGetCurrentDirectory
  found = ra::cli::ParseArgument("OutputGetCurrentDirectory", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentDirectory();
    return 0;
  }

  //validate --OutputGetCurrentDirectory
  found = ra::cli::ParseArgument("OutputGetCurrentDirectory", tmp, argc, argv);
  if (found)
  {
    ra::test::OutputGetCurrentDirectory();
    return 0;
  }

  //validate --SaveGetCurrentProcessPathUtf8
  found = ra::cli::ParseArgument("SaveGetCurrentProcessPathUtf8", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentProcessPathUtf8();
    return (saved ? 0 : 1);
  }

  //validate --SaveGetCurrentProcessDirUtf8
  found = ra::cli::ParseArgument("SaveGetCurrentProcessDirUtf8", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentProcessDirUtf8();
    return (saved ? 0 : 1);
  }

  //validate --SaveGetCurrentDirectoryUtf8
  found = ra::cli::ParseArgument("SaveGetCurrentDirectoryUtf8", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentDirectoryUtf8();
    return (saved ? 0 : 1);
  }
  
  //validate --SaveGetCurrentProcessPath
  found = ra::cli::ParseArgument("SaveGetCurrentProcessPath", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentProcessPath();
    return (saved ? 0 : 1);
  }

  //validate --SaveGetCurrentProcessDir
  found = ra::cli::ParseArgument("SaveGetCurrentProcessDir", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentProcessDir();
    return (saved ? 0 : 1);
  }

  //validate --SaveGetCurrentDirectory
  found = ra::cli::ParseArgument("SaveGetCurrentDirectory", tmp, argc, argv);
  if (found)
  {
    bool saved = ra::test::SaveGetCurrentDirectory();
    return (saved ? 0 : 1);
  }

  //define default values for xml output report
  std::string outputXml = "xml:" "rapidassist_unittest";
  outputXml += (ra::environment::IsProcess32Bit() ? ".x86" : ".x64");
  outputXml += (ra::environment::IsConfigurationDebug() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  //Disable tests that does not run properly on Continuous Integration (CI) server
  if (ra::testing::IsAppVeyor() ||
      ra::testing::IsTravis() ||
      ra::testing::IsGitHub()) {
    std::string basefilter = ::testing::GTEST_FLAG(filter);

    printf("*** Running unit tests on a Continuous Integration (CI) server ***\n");

    //Most CI framework runs in timezone +0 which is not expected by the test.
    printf("*** Disabling TestTiming.testGetUtcTime unit tests ***\n");
    std::string newFilter = ra::testing::MergeFilter("", "TestTiming.testGetUtcTime", basefilter.c_str());

    //AppVeyor does not like console tests. They must be not executing inside a console.
    //AppVeyor reported failure: GetConsoleScreenBufferInfo() error: (6), function 'ra::console::GetCursorPos', line 79.
    //I guess that it is because standard output is beeing redirected for the purpose of logging the program output.
    //
    //Travis CI does not like console tests either. The program hang or enters an infinite loop. Don't really know.
    //Travis reported failure: 
    //    No output has been received in the last 10m0s, this potentially indicates a stalled build or something wrong with the build itself.
    //    Check the details on how to adjust your build configuration on: https://docs.travis-ci.com/user/common-build-problems/#Build-times-out-because-no-output-was-received
    //    The build has been terminated
    //
    //GitHub does not like console tests either. The program outputs the following error when calling GetCursorPos:
    //    GetConsoleScreenBufferInfo() error: (6), function 'ra::console::GetCursorPos', line 76
    //
    //Disabling all console tests.
    printf("*** Disabling TestConsole.* unit tests ***\n");
    newFilter = ra::testing::MergeFilter("", "TestConsole.*", newFilter.c_str());

    ::testing::GTEST_FLAG(filter) = newFilter;
  }

  int wResult = RUN_ALL_TESTS(); //Find and run all tests
  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
