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

#include "TestErrors.h"
#include "rapidassist/errors.h"

#ifdef _WIN32
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#else
#include <cmath>    //for std::log
#include <dirent.h> //for opendir()
#endif

namespace ra { namespace errors { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestErrors::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestErrors::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestErrors, testErrorsDescriptions) {
    for (errorcode_t i = 0; i < 10; i++) {
      std::string desc = ra::errors::GetErrorCodeDescription(i);
      ASSERT_TRUE(!desc.empty());
      printf("Error code 0x%x: %s\n", i, desc.c_str());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestErrors, testRaisingErrorCode) {
    ra::errors::ResetLastErrorCode();

#ifdef _WIN32
    {
      const char * file_path = "c:\\foo\\bar\\baz\\create_file_not_found\\myfile.dat";
      HANDLE hFile = CreateFileA(
        file_path,              // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_NEW,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template
    }
#elif defined(__linux__)
    double not_a_number = std::log(-1.0);
#elif defined(__APPLE__)
    DIR *dp = opendir("/i_do_not_exists");
#endif

    errorcode_t error_code = ra::errors::GetLastErrorCode();
    std::string desc = ra::errors::GetErrorCodeDescription(error_code);
    ASSERT_NE(0, error_code) << "Failed. The function is expecting a non-zero error code. Received error_code " << error_code << ", '" << desc << "'.";
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace errors
} //namespace ra
