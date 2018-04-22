#ifndef TEST_RA_FILESYSTEM_H
#define TEST_RA_FILESYSTEM_H

#include <gtest/gtest.h>

namespace ra { namespace filesystem { namespace test
{
  class TestFilesystem : public ::testing::Test
  {
  public:
    virtual void SetUp();
    virtual void TearDown();
  };

} //namespace test
} //namespace filesystem
} //namespace ra

#endif //TEST_RA_FILESYSTEM_H
