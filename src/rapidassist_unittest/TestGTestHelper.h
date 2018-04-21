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

} // End namespace test
} // End namespace ra

#endif //TESTGTESTHELPER_H