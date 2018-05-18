#include "TestLogger.h"
#include "rapidassist/logger.h"

namespace ra { namespace logger { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestLogger::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestLogger::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogger, testLoggerLevels)
  {
    logger::setQuietMode(false);
    logger::log(logger::LOG_INFO,     "This is information at line=%d.", __LINE__);
    logger::log(logger::LOG_WARNING,  "This is a warning at line=%d.", __LINE__);
    logger::log(logger::LOG_ERROR,    "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogger, testQuietMode)
  {
    logger::setQuietMode(true);
    logger::log(logger::LOG_INFO,     "This is information at line=%d.", __LINE__);
    logger::log(logger::LOG_WARNING,  "This is a warning at line=%d.", __LINE__);
    logger::log(logger::LOG_ERROR,    "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
