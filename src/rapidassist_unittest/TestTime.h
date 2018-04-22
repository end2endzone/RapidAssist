#ifndef TEST_RA_TIME_H
#define TEST_RA_TIME_H

#include <gtest/gtest.h>

namespace ra { namespace time { namespace test
{
  class TestTime : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace time
} //namespace ra

#endif //TESTNATIVEFUNC_H
