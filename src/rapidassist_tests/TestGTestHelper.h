#pragma once

#include <gtest/gtest.h>

class TestGTestHelper : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};
