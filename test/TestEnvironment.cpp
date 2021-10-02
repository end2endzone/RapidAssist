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
  void TestEnvironment::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetEnvironmentVariable) {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::GetEnvironmentVariable(NULL);
      ASSERT_EQ(EXPECTED, actual);
    }

    //test empty string
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::GetEnvironmentVariable("");
      ASSERT_EQ(EXPECTED, actual);
    }

    //test not found
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::GetEnvironmentVariable("FOO_BAR_BIG_BANG");
      ASSERT_EQ(EXPECTED, actual);
    }

    //test a known variable
#ifdef _WIN32
    std::string actual = environment::GetEnvironmentVariable("TEMP");
#elif defined(__linux__) || defined(__APPLE__)
    //Note: "SHELL" variable is not supported in Github Actions.
    //Possible candidates are: LANG, HOME, USER, TERM
    std::string actual = environment::GetEnvironmentVariable("TERM");
#endif
    ASSERT_NE("", actual);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testSetEnvironmentVariable) {
    const char * name = "RAPIDASSIST_FOO";

    //test basic set
    {
      //make sure the variable **is not** defined
      std::string actual = environment::GetEnvironmentVariable(name);
      ASSERT_EQ("", actual);

      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR"));
      ASSERT_EQ("BAR", environment::GetEnvironmentVariable(name));
    }

    //test delete
    {
      //make sure the variable **is** defined
      std::string actual = environment::GetEnvironmentVariable(name);
      ASSERT_NE("", actual);

      ASSERT_TRUE(environment::SetEnvironmentVariable(name, ""));
      ASSERT_EQ("", environment::GetEnvironmentVariable(name));
    }

    //test NULL
    {
      ASSERT_FALSE(environment::SetEnvironmentVariable((const char *)(NULL), (const char *)(NULL)));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, (const char *)(NULL)));
    }

    //test empty string
    {
      ASSERT_FALSE(environment::SetEnvironmentVariable("", (const char *)(NULL)));
    }

    //test override
    {
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR1"));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR2"));

      ASSERT_EQ("BAR2", environment::GetEnvironmentVariable(name));
    }

    //test small integer
    {
      const int value = 42;
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR1"));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, value));

      ASSERT_EQ("42", environment::GetEnvironmentVariable(name));
    }

    //test big integers
    {
      const int64_t value = 9223372036854775807;
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR1"));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, value));

      ASSERT_EQ("9223372036854775807", environment::GetEnvironmentVariable(name));
    }

    //test float
    {
      const float value = 1.2f; // that is "1.20000005". Should be rounded to 1.2.
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR1"));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, value));

      ASSERT_EQ("1.2", environment::GetEnvironmentVariable(name));
    }

    //test double
    {
      const float value = 1.7f;
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, "BAR1"));
      ASSERT_TRUE(environment::SetEnvironmentVariable(name, value));

      ASSERT_EQ("1.7", environment::GetEnvironmentVariable(name));
    }

    //delete variable for next tests
    ASSERT_TRUE(environment::SetEnvironmentVariable(name, ""));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testProcessXXBit) {
    if (environment::IsProcess32Bit()) {
      ASSERT_FALSE(environment::IsProcess64Bit());
      ASSERT_EQ(4, sizeof(void*));
    }
    else if (environment::IsProcess64Bit()) {
      ASSERT_FALSE(environment::IsProcess32Bit());
      ASSERT_EQ(8, sizeof(void*));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetLineSeparator) {
    const char * separator = environment::GetLineSeparator();
    ASSERT_TRUE(separator != NULL);
    ASSERT_TRUE(!std::string(separator).empty());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetEnvironmentVariablesSearch) {
    ra::strings::StringVector variables = environment::GetEnvironmentVariables();
    ASSERT_GT(variables.size(), (size_t)0);

    //find 3 expected names in the list
    bool found1 = false;
    bool found2 = false;
    bool found3 = false;
#ifdef _WIN32
    static const char * variable1 = "USERNAME";
    static const char * variable2 = "TEMP";
    static const char * variable3 = "Path";
#else
    static const char * variable1 = "USER";
    static const char * variable2 = "HOME";
    static const char * variable3 = "PATH";
#endif
    std::string variable_list; //build a list of all found variables in case of a failure.
    for (size_t i = 0; i < variables.size(); i++) {
      const std::string & value = variables[i];

      //build a list of all found variables in case of a failure.
      if (!variable_list.empty())
        variable_list += "\n";
      variable_list += ra::strings::ToString(i) + "=" + value;

      //look for expected names
      if (value == variable1)  found1 = true;
      if (value == variable2)  found2 = true;

      //Note: In a windows console, the variable is named 'Path' but when unit tests are launched from Visual Studio, the name is 'PATH'.
      if (value == variable3 || value == ra::strings::Uppercase(variable3)) found3 = true;
    }

    ASSERT_TRUE(found1) << "The environment variable '" << variable1 << "' was not found in the list of variables:\n" << variable_list.c_str();
    ASSERT_TRUE(found2) << "The environment variable '" << variable2 << "' was not found in the list of variables:\n" << variable_list.c_str();
    ASSERT_TRUE(found3) << "The environment variable '" << variable3 << "' was not found in the list of variables:\n" << variable_list.c_str();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testExpand) {
    //Expand strings that contains 3 expected variable names
#ifdef _WIN32
    static const char * variable1 = "%USERNAME%";
    static const char * variable2 = "%TEMP%";
    static const char * variable3 = "%Path%";
#else
    static const char * variable1 = "$USER";
    static const char * variable2 = "$HOME";
    static const char * variable3 = "$PATH";
#endif

    std::string raw_string1 = std::string("<") + variable1 + ">";
    std::string raw_string2 = std::string("<") + variable2 + ">";
    std::string raw_string3 = std::string("<") + variable3 + ">";

    std::string expanded_string1 = ra::environment::Expand(raw_string1);
    std::string expanded_string2 = ra::environment::Expand(raw_string2);
    std::string expanded_string3 = ra::environment::Expand(raw_string3);

    ASSERT_NE(raw_string1, expanded_string1) << "raw_string1=" << raw_string1.c_str();
    ASSERT_NE(raw_string2, expanded_string2) << "raw_string2=" << raw_string2.c_str();
    ASSERT_NE(raw_string3, expanded_string3) << "raw_string3=" << raw_string3.c_str();

  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
