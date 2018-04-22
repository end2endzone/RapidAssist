#ifndef TESTRANDOM_H
#define TESTRANDOM_H

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

#endif //TESTRANDOM_H
