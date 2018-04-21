#ifndef TESTGTESTHELPER_H
#define TESTGTESTHELPER_H

#include <gtest/gtest.h>

namespace rapidassist { namespace test {

  class TestGTestHelper : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} // End namespace test
} // End namespace rapidassist

#endif //TESTGTESTHELPER_H