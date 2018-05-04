#include <stdio.h>
#include <iostream>
#include <stdio.h>

#include <gtest/gtest.h>

#include "gtesthelp.h"

using namespace ra;

int main(int argc, char **argv)
{
  //define default values for xml output report
  std::string outputXml = "xml:" "rapidassist_unittest";
  outputXml += (ra::gtesthelp::isProcessorX86() ? ".x86" : ".x64");
  outputXml += (ra::gtesthelp::isDebugCode() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  //Disable TestTime.testGetUtcTime() on AppVeyor or Travis CI
  if (ra::gtesthelp::isAppVeyor() || ra::gtesthelp::isTravis())
  {
    //AppVeyor and Travis CI runs in timezone +0 which is not expected by the test.
    std::string newFilter = ra::gtesthelp::mergeFilter("", "TestTime.testGetUtcTime", ::testing::GTEST_FLAG(filter).c_str());
    ::testing::GTEST_FLAG(filter) = newFilter;
  }

  int wResult = RUN_ALL_TESTS(); //Find and run all tests
  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
