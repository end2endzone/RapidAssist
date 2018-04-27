#include <stdio.h>
#include <iostream>
#include <stdio.h>

#include <gtest/gtest.h>

#include "gtesthelper.h"

using namespace ra;

int main(int argc, char **argv)
{
  //define default values for xml output report
  gTestHelper & hlp = gTestHelper::getInstance();
  std::string outputXml = "xml:" "rapidassist_unittest";
  outputXml += (hlp.isProcessorX86() ? ".x86" : ".x64");
  outputXml += (hlp.isDebugCode() ? ".debug" : ".release");
  outputXml += ".xml";
  ::testing::GTEST_FLAG(output) = outputXml;

  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  //Disable TestTime.testGetUtcTime() on AppVeyor or Travis CI
  if (hlp.isAppVeyor() || hlp.isTravis())
  {
    //AppVeyor and Travis CI runs in timezone +0 which is not expected by the test.
    std::string newFilter = hlp.mergeFilter("", "TestTime.testGetUtcTime", ::testing::GTEST_FLAG(filter).c_str());
    ::testing::GTEST_FLAG(filter) = newFilter;
  }

  int wResult = RUN_ALL_TESTS(); //Find and run all tests
  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
