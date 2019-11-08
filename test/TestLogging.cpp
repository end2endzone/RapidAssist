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

#include "TestLogging.h"
#include "rapidassist/logging.h"

namespace ra { namespace logging { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestLogging::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestLogging::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogging, testLoggerLevels) {
    logging::SetQuietMode(false);
    logging::Log(logging::LOG_INFO, "This is information at line=%d.", __LINE__);
    logging::Log(logging::LOG_WARNING, "This is a warning at line=%d.", __LINE__);
    logging::Log(logging::LOG_ERROR, "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogging, testQuietMode) {
    logging::SetQuietMode(true);
    logging::Log(logging::LOG_INFO, "This is information at line=%d.", __LINE__);
    logging::Log(logging::LOG_WARNING, "This is a warning at line=%d.", __LINE__);
    logging::Log(logging::LOG_ERROR, "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
