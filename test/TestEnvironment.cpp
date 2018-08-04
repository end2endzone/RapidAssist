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

#include "TestEnvironment.h"
#include "rapidassist/environment.h"

namespace ra { namespace environment { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetEnvironmentVariable)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable(NULL);
      ASSERT_EQ(EXPECTED, actual);
    }

    //test empty string
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("");
      ASSERT_EQ(EXPECTED, actual);
    }

    //test not found
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("FOO_BAR_BIG_BANG");
      ASSERT_EQ(EXPECTED, actual);
    }

#ifdef WIN32
    //test TEMP
    {
      std::string actual = environment::getEnvironmentVariable("TEMP");
      ASSERT_NE("", actual);
    }
#elif __linux__
    //test SHELL
    {
      std::string actual = environment::getEnvironmentVariable("SHELL");
      ASSERT_NE("", actual);
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testProcessXXBit)
  {
    if (environment::isProcess32Bit())
    {
      ASSERT_FALSE(environment::isProcess64Bit());
      ASSERT_EQ(4, sizeof(void*));
    }
    else if (environment::isProcess64Bit())
    {
      ASSERT_FALSE(environment::isProcess32Bit());
      ASSERT_EQ(8, sizeof(void*));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetLineSeparator)
  {
    const char * separator = environment::getLineSeparator();
    ASSERT_TRUE( separator != NULL );
    ASSERT_TRUE( !std::string(separator).empty() );
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
