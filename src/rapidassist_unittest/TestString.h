#ifndef TEST_RA_STRING_H
#define TEST_RA_STRING_H

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

#endif //TEST_RA_STRING_H
