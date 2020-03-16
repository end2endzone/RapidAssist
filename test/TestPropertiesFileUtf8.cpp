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

#include "TestPropertiesFileUtf8.h"

#include "rapidassist/propertiesfile.h"

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing_utf8.h"

namespace ra { namespace filesystem { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestPropertiesFileUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestPropertiesFileUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertiesFileUtf8, testLoadUtf8) {
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string file_source_path_nonutf8  = std::string("test_files") + path_separator + test_name + ".txt";
    std::string file_source_path_utf8     = std::string("test_files") + path_separator + test_name + ".psi_\xCE\xA8_psi.txt";

    bool copied = ra::filesystem::CopyFileUtf8(file_source_path_nonutf8, file_source_path_utf8);
    ASSERT_TRUE(copied);

    PropertiesFile s;
    ASSERT_TRUE(s.LoadUtf8(file_source_path_utf8));

    //assert expected values
    const std::string key01 = "website";
    const std::string key02 = "language";
    const std::string key03 = "message";
    const std::string key04 = "key with spaces";
    const std::string key05 = "tab";
    const std::string key06 = "path";
    const std::string expected01 = "https://en.wikipedia.org/";
    const std::string expected02 = "English";
    const std::string expected03 = "Welcome to Wikipedia!";
    const std::string expected04 = "This is the value that could be looked up with the key \"key with spaces\".";
    const std::string expected05 = "\\u0009"; //unicode characters not supported by PropertiesFile class. 
    const std::string expected06 = "c:\\wiki\\templates";
    std::string actual01;
    std::string actual02;
    std::string actual03;
    std::string actual04;
    std::string actual05;
    std::string actual06;
    ASSERT_TRUE(s.GetValue(key01, actual01));
    ASSERT_TRUE(s.GetValue(key02, actual02));
    ASSERT_TRUE(s.GetValue(key03, actual03));
    ASSERT_TRUE(s.GetValue(key04, actual04));
    ASSERT_TRUE(s.GetValue(key05, actual05));
    ASSERT_TRUE(s.GetValue(key06, actual06));

    ASSERT_EQ(expected01, actual01);
    ASSERT_EQ(expected02, actual02);
    ASSERT_EQ(expected03, actual03);
    ASSERT_EQ(expected04, actual04);
    ASSERT_EQ(expected05, actual05);
    ASSERT_EQ(expected06, actual06);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestPropertiesFileUtf8, testSaveUtf8) {
    static const std::string path_separator = ra::filesystem::GetPathSeparatorStr();
    std::string test_name = ra::testing::GetTestQualifiedName();
    std::string file_expected_path = std::string("test_files") + path_separator + test_name + ".expected.txt";
    std::string file_actual_path_nonutf8  = std::string("test_files") + path_separator + test_name + ".actual.txt";
    std::string file_actual_path_utf8     = std::string("test_files") + path_separator + test_name + ".actual.psi_\xCE\xA8_psi.txt";

    PropertiesFile s;

    const std::string key01 = "website";
    const std::string key02 = "language";
    const std::string key03 = "key with spaces";
    const std::string key04 = "key=with=equals";
    const std::string key05 = "key:with:colons";
    const std::string value01 = "https://en.wikipedia.org/";
    const std::string value02 = "English";
    const std::string value03 = "this is the value of \"key with spaces\".";
    const std::string value04 = "this is the value of \"key=with=equals\".";
    const std::string value05 = "this is the value of \"key:with:colons\".";

    s.SetValue(key01, value01);
    s.SetValue(key02, value02);
    s.SetValue(key03, value03);
    s.SetValue(key04, value04);
    s.SetValue(key05, value05);

    ASSERT_TRUE(s.SaveUtf8(file_actual_path_utf8)) << "Failed saving properties to file '" << file_actual_path_utf8 << "'.";

    //assert that loading the saved file results in the same properties
    PropertiesFile s2;
    ASSERT_TRUE(s2.LoadUtf8(file_actual_path_utf8)) << "Failed loading property file '" << file_actual_path_utf8 << "'.";
    std::string read01;
    std::string read02;
    std::string read03;
    std::string read04;
    std::string read05;
    ASSERT_TRUE(s2.GetValue(key01, read01)) << "Failed reading the key '" << key01 << "' from file '" << file_actual_path_utf8 << "'.";
    ASSERT_TRUE(s2.GetValue(key02, read02)) << "Failed reading the key '" << key02 << "' from file '" << file_actual_path_utf8 << "'.";
    ASSERT_TRUE(s2.GetValue(key03, read03)) << "Failed reading the key '" << key03 << "' from file '" << file_actual_path_utf8 << "'.";
    ASSERT_TRUE(s2.GetValue(key04, read04)) << "Failed reading the key '" << key04 << "' from file '" << file_actual_path_utf8 << "'.";
    ASSERT_TRUE(s2.GetValue(key05, read05)) << "Failed reading the key '" << key05 << "' from file '" << file_actual_path_utf8 << "'.";
    ASSERT_EQ(value01, read01);
    ASSERT_EQ(value02, read02);
    ASSERT_EQ(value03, read03);
    ASSERT_EQ(value04, read04);
    ASSERT_EQ(value05, read05);
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace filesystem
} //namespace ra
