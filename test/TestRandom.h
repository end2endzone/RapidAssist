#ifndef TEST_RA_RANDOM_H
#define TEST_RA_RANDOM_H

#include <gtest/gtest.h>

namespace ra { namespace random { namespace test
{
  class TestRandom : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace random
} //namespace ra

#endif //TEST_RA_RANDOM_H
