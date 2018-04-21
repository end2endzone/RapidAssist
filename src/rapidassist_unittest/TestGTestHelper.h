#ifndef TESTGTESTHELPER_H
#define TESTGTESTHELPER_H

#include <gtest/gtest.h>

class TestGTestHelper : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTGTESTHELPER_H