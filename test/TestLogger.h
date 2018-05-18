#ifndef TEST_RA_LOGGER_H
#define TEST_RA_LOGGER_H

#include <gtest/gtest.h>

namespace ra { namespace logger { namespace test
{
  class TestLogger : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace logger
} //namespace ra

#endif //TEST_RA_LOGGER_H
