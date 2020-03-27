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
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing_utf8.h"

namespace ra { namespace test
{
  static const std::string separator = ra::filesystem::GetPathSeparatorStr();

  std::string GetFunctionNameWithoutNamespace(const char * full_name) {
    ra::strings::StringVector values;
    ra::strings::Split(values, full_name, ":");
    if (values.empty())
      return "";
    const std::string & last = values[values.size()-1];
    return last;
  }

  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessPathUtf8() {
    std::string current_process_path_utf8 = ra::process::GetCurrentProcessPathUtf8();
    printf("%s\n", current_process_path_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessDirUtf8() {
    std::string current_process_dir_utf8 = ra::process::GetCurrentProcessDirUtf8();
    printf("%s\n", current_process_dir_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentDirectoryUtf8() {
    std::string current_dir_utf8 = ra::filesystem::GetCurrentDirectoryUtf8();
    printf("%s\n", current_dir_utf8.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessPath() {
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    printf("%s\n", current_process_path.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentProcessDir() {
    std::string current_process_dir = ra::process::GetCurrentProcessDir();
    printf("%s\n", current_process_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  void OutputGetCurrentDirectory() {
    std::string current_dir = ra::filesystem::GetCurrentDirectory();
    printf("%s\n", current_dir.c_str());
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessPathUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_path_utf8 = ra::process::GetCurrentProcessPathUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_process_path_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessDirUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_dir_utf8 = ra::process::GetCurrentProcessDirUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_process_dir_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentDirectoryUtf8() {
    const std::string file_path = ra::process::GetCurrentProcessDirUtf8() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_dir_utf8 = ra::filesystem::GetCurrentDirectoryUtf8();
    bool saved = ra::filesystem::WriteTextFileUtf8(file_path, current_dir_utf8);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessPath() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_path_ = ra::process::GetCurrentProcessPath();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_process_path_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentProcessDir() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_process_dir_ = ra::process::GetCurrentProcessDir();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_process_dir_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------
  bool SaveGetCurrentDirectory() {
    const std::string file_path = ra::process::GetCurrentProcessDir() + separator + GetFunctionNameWithoutNamespace(__FUNCTION__) + ".txt";
    const std::string current_dir_ = ra::filesystem::GetCurrentDirectory();
    bool saved = ra::filesystem::WriteTextFile(file_path, current_dir_);
    return saved;
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace ra
