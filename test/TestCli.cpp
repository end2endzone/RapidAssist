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

#include "TestCli.h"
#include "rapidassist/cli.h"
#include "rapidassist/strings.h"

namespace ra { namespace cli { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestCli::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestCli::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestCli, testParseArgumentString) {
    const char * arguments[] = { "myapp", "--arg1=value1", "--test2=value2", "--quick", "--number=12345", "--big=4123456789", "--foo=bar", "--last=final", "" };
    int argc = sizeof(arguments) / sizeof(arguments[0]) - 1; //the last element must be NULL and not part of the argc value.

    //must const cast because argv is supplied as a `char**` instead of `const char **`
    char ** argv = (char**)arguments;

    //test flag/switch
    {
      static const std::string name = "quick";
      std::string value;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "");
    }

    //test first
    {
      static const std::string name = "arg1";
      std::string value;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "value1");
    }

    //test last
    {
      static const std::string name = "last";
      std::string value;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "final");
    }

    //test middle
    {
      static const std::string name = "foo";
      std::string value;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "bar");
    }

    //test int
    {
      static const std::string name = "number";
      int value = 0;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, 12345);
    }

    //test size_t
    {
      static const std::string name = "big";
      size_t value = 0;
      bool found = cli::ParseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, 4123456789ul);
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace cli
} //namespace ra
