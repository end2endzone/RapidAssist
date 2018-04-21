#ifndef TESTFILESYSTEMFUNC_H
#define TESTFILESYSTEMFUNC_H

#include <gtest/gtest.h>

namespace rapidassist { namespace filesystem { namespace test
{
  class TestFilesystemFunc : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} // End namespace test
} // End namespace filesystem
} // End namespace rapidassist

#endif //TESTFILESYSTEMFUNC_H
