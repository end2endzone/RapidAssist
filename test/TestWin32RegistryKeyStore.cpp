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

#include "TestWin32RegistryKeyStore.h"

#include "rapidassist/keystore.h"

#include "rapidassist/environment.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/gtesthelp.h"

namespace ra
{
namespace keystore
{
namespace test
{
  static const std::string ENVIRONMENT_REGISTRY_PATH = "HKEY_CURRENT_USER\\Environment";

  //--------------------------------------------------------------------------------------------------
  void TestWin32RegistryKeyStore::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestWin32RegistryKeyStore::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testLoad)
  {
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load(ENVIRONMENT_REGISTRY_PATH.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testGetValue)
  {
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load(ENVIRONMENT_REGISTRY_PATH.c_str()) );

    std::string value01;
    std::string value02;
    std::string value03;
    ASSERT_TRUE( s.getValue("Path", value01) );
    ASSERT_TRUE( s.getValue("TEMP", value02) );
    ASSERT_TRUE( s.getValue("TMP" , value03) );

    int a = 0;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testSetValue)
  {
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load(ENVIRONMENT_REGISTRY_PATH.c_str()) );

    ASSERT_TRUE( s.setValue("foo", "bar") );

    std::string value01;
    ASSERT_TRUE( s.getValue("foo", value01) );

    int a = 0;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testClear)
  {
    const std::string temp_dir = ra::environment::getEnvironmentVariable("TEMP");
    const std::string temp_file = temp_dir + "\\tmp.reg";

    //create a fake registry key 'Environment2'
    printf("Creating temporary environment 'HKEY_CURRENT_USER\\Environment2'...\n");
    const std::string key = "HKEY_CURRENT_USER\\Environment2";
    const std::string test_name = ra::gtesthelp::getTestQualifiedName();
    ra::strings::StringVector commands;
    commands.push_back(std::string() + "reg ADD " + key + " /f");
    commands.push_back(std::string() + "reg ADD " + key + " /f /v foo /t REG_SZ /d bar");
    commands.push_back(std::string() + "reg ADD " + key + " /f /v message /t REG_SZ /d \"hello world from " + test_name + "\"");
    commands.push_back(std::string() + "reg ADD " + key + " /f /v name /t REG_SZ /d \"John Smith\"");
    commands.push_back(std::string() + "reg ADD " + key + " /f /v language /t REG_SZ /d en");
    for(size_t i=0; i<commands.size(); i++)
    {
      const std::string & command = commands[i];
      int return_code = system(command.c_str());
      ASSERT_EQ(0, return_code) << "Failed runnung command:\n" << command << "\nReturn code: " << return_code;
    }

    //assert values are properly created
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load("HKEY_CURRENT_USER\\Environment2") );
    ASSERT_TRUE( s.hasKey("foo") );
    ASSERT_TRUE( s.hasKey("message") );
    ASSERT_TRUE( s.hasKey("name") );
    ASSERT_TRUE( s.hasKey("language") );

    //act
    ASSERT_TRUE( s.clear() );

    //assert all registry values was deleted
    ASSERT_FALSE( s.hasKey("foo") );
    ASSERT_FALSE( s.hasKey("message") );
    ASSERT_FALSE( s.hasKey("name") );
    ASSERT_FALSE( s.hasKey("language") );

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testDeleteKey)
  {
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load(ENVIRONMENT_REGISTRY_PATH.c_str()) );

    static const std::string KEY = "foo";

    ASSERT_TRUE( s.setValue(KEY, "bar") );
    ASSERT_TRUE( s.deleteKey(KEY) );

    int a = 0;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestWin32RegistryKeyStore, testHasKey)
  {
    Win32RegistryKeyStore s;
    ASSERT_TRUE( s.load(ENVIRONMENT_REGISTRY_PATH.c_str()) );

    static const std::string KEY = "mykey";

    ASSERT_FALSE( s.hasKey(KEY) );
    ASSERT_TRUE( s.setValue(KEY, "myvalue") );
    ASSERT_TRUE( s.hasKey(KEY) );

    //cleanup
    ASSERT_TRUE( s.deleteKey(KEY) );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace keystore
} //namespace ra
