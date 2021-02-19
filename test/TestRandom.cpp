/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "TestRandom.h"
#include "rapidassist/random.h"

namespace ra { namespace random { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestRandom::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestRandom::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestRandom, testGetRandomInt) {
    int previous = 0;
    for (size_t i = 0; i < 100; i++) {
      int newValue = GetRandomInt();
      ASSERT_NE(previous, newValue);
      previous = newValue;
    }
  }
  //--------------------------------------------------------------------------------------------------
  bool IsAllTrue(bool * my_array, size_t size) {
    for (size_t i = 0; i < size; i++) {
      if (my_array[i] == false)
        return false;
    }
    return true;
  }
  TEST_F(TestRandom, testGetRandomIntRange) {
    //assert that 10000 random number between -50 and +49 should returns all possible values
    {
      static const int RANGE_MIN = -50;
      static const int RANGE_MAX = +49;
      bool found[100] = { 0 };  //from [-50, +49] is like [0, +99], which is 100 different values
      for (size_t i = 0; i < 10000; i++) {
        int value = GetRandomInt(RANGE_MIN, RANGE_MAX);
        ASSERT_GE(value, RANGE_MIN);
        ASSERT_LE(value, RANGE_MAX);

        int index = value + 50;
        ASSERT_GE(index, 0);
        ASSERT_LE(index, 99);

        found[index] = true;
        if (IsAllTrue(found, sizeof(found))) {
          break;
        }
      }

      ASSERT_TRUE(IsAllTrue(found, sizeof(found)));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestRandom, testGetRandomString) {
    ASSERT_TRUE(!GetRandomString().empty());

    ASSERT_TRUE(GetRandomString(0).empty());

    //assert maxlen
    for (size_t i = 0; i < 10; i++) {
      std::string s = GetRandomString(i);
      ASSERT_EQ(i, s.size());
    }

    //assert symbols
    static const std::string SYMBOLS = "0123abc";
    for (size_t i = 0; i < 1000; i++) {
      static const size_t LENGTH = 20;
      std::string s = GetRandomString(LENGTH, SYMBOLS.c_str());
      ASSERT_EQ(LENGTH, s.size());

      for (size_t j = 0; j < s.size(); j++) {
        ASSERT_TRUE(
          s[j] == '0' ||
          s[j] == '1' ||
          s[j] == '2' ||
          s[j] == '3' ||
          s[j] == 'a' ||
          s[j] == 'b' ||
          s[j] == 'c');
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace random
} //namespace ra
