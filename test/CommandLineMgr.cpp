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

#include "CommandLineMgr.h"

#include "rapidassist/cli.h"
#include "rapidassist/process_utf8.h"

namespace ra { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void OutputProcessProperties() {
    std::string current_process_path_utf8 = ra::process::GetCurrentProcessPathUtf8();
    std::string current_process_dir_utf8 = ra::process::GetCurrentProcessDirUtf8();

    printf("current_process_path_utf8=%s\n", current_process_path_utf8.c_str());
    printf("current_process_dir_utf8=%s\n", current_process_dir_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace ra
