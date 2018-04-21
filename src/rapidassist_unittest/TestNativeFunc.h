#ifndef TESTNATIVEFUNC_H
#define TESTNATIVEFUNC_H

#include <gtest/gtest.h>

namespace rapidassist { namespace nativefunc { namespace test
{
  class TestNativeFunc : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} // End namespace test
} // End namespace nativefunc
} // End namespace rapidassist

#endif //TESTNATIVEFUNC_H
