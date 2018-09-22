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

#include "TestGenerics.h"
#include "rapidassist/generics.h"

namespace ra { namespace generics { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestGenerics::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestGenerics::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testSwapInt)
  {
    int a = 5;
    int b = 9;
    ra::generics::swap(a,b);

    ASSERT_EQ(5, b);
    ASSERT_EQ(9, a);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testSwapString)
  {
    std::string a = "abc";
    std::string b = "def";
    ra::generics::swap(a,b);

    ASSERT_EQ("abc", b);
    ASSERT_EQ("def", a);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMinimum)
  {
    int a = 5;
    int b = 9;

    ASSERT_EQ(5, ra::generics::minimum(a,b) );
    ASSERT_EQ(5, ra::generics::minimum(b,a) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMaximum)
  {
    int a = 5;
    int b = 9;

    ASSERT_EQ(9, ra::generics::maximum(a,b) );
    ASSERT_EQ(9, ra::generics::maximum(b,a) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testConstrain)
  {
    int low = 10;
    int high = 20;

    ASSERT_EQ(low, ra::generics::constrain(5, low, high) );
    ASSERT_EQ(high, ra::generics::constrain(999, low, high) );
    ASSERT_EQ(15, ra::generics::constrain(15, low, high) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMap)
  {
    int fahrenheit_freeze = 32;
    int fahrenheit_boil = 212;
    int celsius_freeze = 0;
    int celsius_boil = 100;
    
    //celcius to fahrenheit
    ASSERT_EQ( 32, ra::generics::map<int>(  0, celsius_freeze, celsius_boil, fahrenheit_freeze, fahrenheit_boil));
    ASSERT_EQ(212, ra::generics::map<int>(100, celsius_freeze, celsius_boil, fahrenheit_freeze, fahrenheit_boil));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testNear)
  {
    //as integer
    ASSERT_FALSE( ra::generics::near(5, 9, 1) );    //epsilon too small
    ASSERT_FALSE( ra::generics::near(9, 5, 1) );    //epsilon too small
    ASSERT_TRUE ( ra::generics::near(5, 9, 4) );    //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(9, 5, 4) );    //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(5, 9, 400) );  //epsilon too big
    ASSERT_TRUE ( ra::generics::near(9, 5, 400) );  //epsilon too big

    //as float
    ASSERT_FALSE( ra::generics::near(5.0f, 9.0f, 1.0f) );   //epsilon too small
    ASSERT_FALSE( ra::generics::near(9.0f, 5.0f, 1.0f) );   //epsilon too small
    ASSERT_TRUE ( ra::generics::near(5.0f, 9.0f, 4.0f) );   //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(9.0f, 5.0f, 4.0f) );   //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(5.0f, 9.0f, 400.0f) ); //epsilon too big
    ASSERT_TRUE ( ra::generics::near(9.0f, 5.0f, 400.0f) ); //epsilon too big

    //as double
    ASSERT_FALSE( ra::generics::near(5.0, 9.0, 1.0) );    //epsilon too small
    ASSERT_FALSE( ra::generics::near(9.0, 5.0, 1.0) );    //epsilon too small
    ASSERT_TRUE ( ra::generics::near(5.0, 9.0, 4.0) );    //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(9.0, 5.0, 4.0) );    //epsilon is exact
    ASSERT_TRUE ( ra::generics::near(5.0, 9.0, 400.0) );  //epsilon too big
    ASSERT_TRUE ( ra::generics::near(9.0, 5.0, 400.0) );  //epsilon too big
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testReadAs)
  {
    //float (32 bit) to integer (32 bit)
    {
      float f = 0.0f;
      int32_t i = 0;
      ASSERT_EQ(i, ra::generics::readAs<int32_t>(f) );

      f = 0.1f;
      i = 1036831949;
      ASSERT_EQ(i, ra::generics::readAs<int32_t>(f) );

      f = 100000.0f;
      i = 1203982336;
      ASSERT_EQ(i, ra::generics::readAs<int32_t>(f) );

      f = 1.3958644e+010f;
      i = 1347420160;
      ASSERT_EQ(i, ra::generics::readAs<int32_t>(f) );
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace generics
} //namespace ra
