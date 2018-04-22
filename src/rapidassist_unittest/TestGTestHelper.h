#ifndef TEST_RA_GTESTHELPER_H
#define TEST_RA_GTESTHELPER_H

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

#endif //TEST_RA_GTESTHELPER_H
