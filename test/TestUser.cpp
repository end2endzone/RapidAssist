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

#include "TestUser.h"
#include "rapidassist/user.h"
#include "rapidassist/filesystem.h"

namespace ra { namespace user { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestUser::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestUser::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUser, printValues)
  {
    std::string home_dir = ra::user::getHomeDirectory();
    std::string  app_dir = ra::user::getApplicationsDataDirectory();
    std::string docs_dir = ra::user::getDocumentsDirectory();
    std::string desk_dir = ra::user::getDesktopDirectory();
    std::string username = ra::user::getUsername();

    printf("home_dir=%s\n", home_dir.c_str());
    printf(" app_dir=%s\n",  app_dir.c_str());
    printf("docs_dir=%s\n", docs_dir.c_str());
    printf("desk_dir=%s\n", desk_dir.c_str());
    printf("username=%s\n", username.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUser, testFoldersExisting)
  {
    std::string home_dir = ra::user::getHomeDirectory();
    std::string  app_dir = ra::user::getApplicationsDataDirectory();
    std::string docs_dir = ra::user::getDocumentsDirectory();
    std::string desk_dir = ra::user::getDesktopDirectory();

    ASSERT_FALSE( home_dir.empty() );
    ASSERT_FALSE(  app_dir.empty() );
    ASSERT_FALSE( docs_dir.empty() );
    ASSERT_FALSE( desk_dir.empty() );

    ASSERT_TRUE( ra::filesystem::folderExists(home_dir.c_str()) ) << "Failed. Folder '" << home_dir << "' does not exists!";
    ASSERT_TRUE( ra::filesystem::folderExists( app_dir.c_str()) ) << "Failed. Folder '" <<  app_dir << "' does not exists!";
    ASSERT_TRUE( ra::filesystem::folderExists(docs_dir.c_str()) ) << "Failed. Folder '" << docs_dir << "' does not exists!";
    ASSERT_TRUE( ra::filesystem::folderExists(desk_dir.c_str()) ) << "Failed. Folder '" << desk_dir << "' does not exists!";
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace user
} //namespace ra
