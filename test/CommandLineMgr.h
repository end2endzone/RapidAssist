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

#ifndef TEST_RA_COMMANDLINEMGR_H
#define TEST_RA_COMMANDLINEMGR_H

#include <string>

namespace ra { namespace test
{
  void OutputGetCurrentProcessPathUtf8();
  void OutputGetCurrentProcessDirUtf8();
  void OutputGetCurrentDirectoryUtf8();

  void OutputGetCurrentProcessPath();
  void OutputGetCurrentProcessDir();
  void OutputGetCurrentDirectory();

  bool SaveGetCurrentProcessPathUtf8();
  bool SaveGetCurrentProcessDirUtf8();
  bool SaveGetCurrentDirectoryUtf8();

  bool SaveGetCurrentProcessPath();
  bool SaveGetCurrentProcessDir();
  bool SaveGetCurrentDirectory();

  void WaitForTerminateSignal();
  void WaitForKillSignal();

  void SleepTime(int sleep_time_ms);
  int ExitCode(int exit_code);

} //namespace test
} //namespace ra

#endif //TEST_RA_CLI_H
