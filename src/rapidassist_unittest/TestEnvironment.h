#ifndef TEST_RA_ENVIRONMENT_H
#define TEST_RA_ENVIRONMENT_H

#include <gtest/gtest.h>

namespace ra { namespace environment { namespace test
{
  class TestEnvironment : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace environment
} //namespace ra

#endif //TEST_RA_ENVIRONMENT_H
