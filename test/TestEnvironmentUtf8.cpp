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

#include "TestEnvironmentUtf8.h"
#include "rapidassist/environment_utf8.h"

namespace ra { namespace environment { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestEnvironmentUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestEnvironmentUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironmentUtf8, testSetEnvironmentVariableUtf8) {
    const char * name = "RAPIDASSIST_FOO";

    //test basic set
    {
      //make sure the variable **is not** defined
      std::string actual = environment::GetEnvironmentVariableUtf8(name);
      ASSERT_EQ("", actual);

      ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, "psi_\xCE\xA8_psi"));
      ASSERT_EQ("psi_\xCE\xA8_psi", environment::GetEnvironmentVariableUtf8(name));
#ifdef _WIN32
      //this assertion can only be done on system where GetEnvironmentVariable() and GetEnvironmentVariableUtf8() are different.
      ASSERT_NE("psi_\xCE\xA8_psi", environment::GetEnvironmentVariable(name)); //assert the value is really utf-8 encoded
#endif
    }

    //test delete
    {
      //make sure the variable **is** defined
      std::string actual = environment::GetEnvironmentVariableUtf8(name);
      ASSERT_NE("", actual);

      ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, ""));
      ASSERT_EQ("", environment::GetEnvironmentVariableUtf8(name));
    }

    //test NULL
    {
      ASSERT_FALSE(environment::SetEnvironmentVariableUtf8((const char *)(NULL), (const char *)(NULL)));
      ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, (const char *)(NULL)));
    }

    //test empty string
    {
      ASSERT_FALSE(environment::SetEnvironmentVariableUtf8("", (const char *)(NULL)));
    }

    //test override
    {
      ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, "psi_\xCE\xA8_psi"));
      ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, "copyright_\xC2\xA9_copyright"));

      ASSERT_EQ("copyright_\xC2\xA9_copyright", environment::GetEnvironmentVariableUtf8(name));
    }

    //delete variable for next tests
    ASSERT_TRUE(environment::SetEnvironmentVariable(name, ""));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironmentUtf8, testGetEnvironmentVariablesUtf8Search) {
    ra::strings::StringVector variables = environment::GetEnvironmentVariablesUtf8();
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
  TEST_F(TestEnvironmentUtf8, testGetEnvironmentVariablesUtf8) {
    //assert number of variables is identical.
    ra::strings::StringVector variables = environment::GetEnvironmentVariablesUtf8();
    ra::strings::StringVector tmp = environment::GetEnvironmentVariables();
    ASSERT_GT(variables.size(), (size_t)0);
    ASSERT_EQ(variables.size(), tmp.size());

    //set an utf-8 variable
    const char * name = "RAPIDASSIST_FOO";
    ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, "psi_\xCE\xA8_psi"));

    //assert it can be found in list of variables
    variables = environment::GetEnvironmentVariablesUtf8(); //refresh to get the new variable
    bool found = false;
    for(size_t i=0; i<variables.size(); i++)
    {
      const std::string & name_utf8 = variables[i];
      std::string value_utf8 = environment::GetEnvironmentVariableUtf8(name_utf8.c_str());
      if (name_utf8 == name && value_utf8 == "psi_\xCE\xA8_psi")
        found = true;
    }
    ASSERT_TRUE( found );

    //delete variable for next tests
    ASSERT_TRUE(environment::SetEnvironmentVariable(name, ""));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironmentUtf8, testExpandUtf8) {
    //set an utf-8 variable
    const char * name = "RAPIDASSIST_FOO";
    ASSERT_TRUE(environment::SetEnvironmentVariableUtf8(name, "psi_\xCE\xA8_psi"));

#ifdef _WIN32
    static const char * variable1 = "%RAPIDASSIST_FOO%";
#else
    static const char * variable1 = "$RAPIDASSIST_FOO";
#endif

    std::string content = std::string("<") + variable1 + ">";

    std::string expanded = ra::environment::ExpandUtf8(content);
    std::string tmp = ra::environment::Expand(content);

    //assert that expanded strings are not identical
#ifdef _WIN32
    //this assertion can only be done on system where Expand() and ExpandUtf8() are different.
    ASSERT_NE(expanded, tmp);
#endif

    ASSERT_EQ(expanded, "<psi_\xCE\xA8_psi>") << "expanded=" << expanded.c_str();

    //delete variable for next tests
    ASSERT_TRUE(environment::SetEnvironmentVariable(name, ""));
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
