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

#include "TestUserUtf8.h"
#include "rapidassist/user_utf8.h"
#include "rapidassist/filesystem_utf8.h"

namespace ra { namespace user { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestUserUtf8::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestUserUtf8::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUserUtf8, printValues) {
    std::string home_dir_utf8 = ra::user::GetHomeDirectoryUtf8();
    std::string  app_dir_utf8 = ra::user::GetApplicationsDataDirectoryUtf8();
    std::string docs_dir_utf8 = ra::user::GetDocumentsDirectoryUtf8();
    std::string desk_dir_utf8 = ra::user::GetDesktopDirectoryUtf8();
    std::string username_utf8 = ra::user::GetUsernameUtf8();

    printf("home_dir_utf8: %s\n", home_dir_utf8.c_str());
    printf(" app_dir_utf8: %s\n", app_dir_utf8.c_str());
    printf("docs_dir_utf8: %s\n", docs_dir_utf8.c_str());
    printf("desk_dir_utf8: %s\n", desk_dir_utf8.c_str());
    printf("username_utf8: %s\n", username_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUserUtf8, testDirectoriesExisting) {
    std::string home_dir_utf8 = ra::user::GetHomeDirectoryUtf8();
    std::string  app_dir_utf8 = ra::user::GetApplicationsDataDirectoryUtf8();
    std::string docs_dir_utf8 = ra::user::GetDocumentsDirectoryUtf8();
    std::string desk_dir_utf8 = ra::user::GetDesktopDirectoryUtf8();

    ASSERT_FALSE(home_dir_utf8.empty());
    ASSERT_FALSE(app_dir_utf8.empty());
    ASSERT_FALSE(docs_dir_utf8.empty());
    ASSERT_FALSE(desk_dir_utf8.empty());

    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(home_dir_utf8.c_str())) << "Failed. Directory '" << home_dir_utf8 << "' does not exists!";
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(app_dir_utf8.c_str())) << "Failed. Directory '" << app_dir_utf8 << "' does not exists!";
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(docs_dir_utf8.c_str())) << "Failed. Directory '" << docs_dir_utf8 << "' does not exists!";
    ASSERT_TRUE(ra::filesystem::DirectoryExistsUtf8(desk_dir_utf8.c_str())) << "Failed. Directory '" << desk_dir_utf8 << "' does not exists!";
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace user
} //namespace ra
