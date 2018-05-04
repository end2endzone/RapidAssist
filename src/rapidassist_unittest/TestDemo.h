#ifndef TEST_DEMO_H
#define TEST_DEMO_H

#include <gtest/gtest.h>

namespace ra { namespace test {

  class TestDemo : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace ra

#endif //TEST_DEMO_H
