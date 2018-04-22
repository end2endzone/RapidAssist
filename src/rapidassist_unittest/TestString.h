#ifndef TESTSTRINGFUNC_H
#define TESTSTRINGFUNC_H

#include <gtest/gtest.h>

namespace ra { namespace strings { namespace test
{
  class TestString : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace strings
} //namespace ra

#endif //TESTSTRINGFUNC_H
