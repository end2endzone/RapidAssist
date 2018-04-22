#ifndef TEST_RA_CLI_H
#define TEST_RA_CLI_H

#include <gtest/gtest.h>

namespace ra { namespace cli { namespace test
{
  class TestCli : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace cli
} //namespace ra

#endif //TEST_RA_CLI_H
