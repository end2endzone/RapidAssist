#ifndef TESTENVIRONMENTFUNC_H
#define TESTENVIRONMENTFUNC_H

#include <gtest/gtest.h>

namespace ra { namespace environment { namespace test
{
  class TestEnvironment : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace environment
} //namespace ra

#endif //TESTENVIRONMENTFUNC_H
