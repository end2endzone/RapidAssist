#ifndef TESTGTESTHELPER_H
#define TESTGTESTHELPER_H

#include <gtest/gtest.h>

namespace ra { namespace test {

  class TestGTestHelper : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace ra

#endif //TESTGTESTHELPER_H