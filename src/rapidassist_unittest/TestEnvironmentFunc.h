#ifndef TESTENVIRONMENTFUNC_H
#define TESTENVIRONMENTFUNC_H

#include <gtest/gtest.h>

namespace ra { namespace environment { namespace test
{
  class TestEnvironmentFunc : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} // End namespace test
} // End namespace environment
} // End namespace ra

#endif //TESTENVIRONMENTFUNC_H
