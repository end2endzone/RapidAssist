#ifndef TEST_RA_GTESTHELP_H
#define TEST_RA_GTESTHELP_H

#include <gtest/gtest.h>

namespace ra { namespace test {

  class TestGTestHelp : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace ra

#endif //TEST_RA_GTESTHELP_H
