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

#include <stdlib.h> //for system()
#ifdef __linux__
#include <sys/wait.h> //for WEXITSTATUS
#endif

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
  bool CloneProcess(std::string & working_dir_path, std::string & new_process_path, const bool support_utf8, std::string & error_message) {
    working_dir_path = "";
    new_process_path = "";
    
    const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Create a working directory that matches current test name and contains an utf8 character.
    std::string test_dir_name = ra::testing::GetTestQualifiedName();
    if (support_utf8)
      test_dir_name.append(".psi_\xCE\xA8_psi");
    working_dir_path = ra::process::GetCurrentProcessDir() + separator + test_dir_name;
    bool created = false;
    if (support_utf8)
      created = ra::filesystem::CreateDirectoryUtf8(working_dir_path.c_str());
    else
      created = ra::filesystem::CreateDirectory(working_dir_path.c_str());
    if (!created)
    {
      error_message = "Failed creating directory '" + working_dir_path + "'.";
      return false;
    }

    //clone current process executable into another process which name contains an utf8 character.
    std::string new_process_filename = ra::testing::GetTestQualifiedName();
    if (support_utf8)
      new_process_filename.append(".omega_\xCE\xA9_omega");
#ifdef _WIN32
    new_process_filename.append(".exe");
#endif
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    new_process_path = working_dir_path + separator + new_process_filename;
    bool copied = false;
    if (support_utf8)
      copied = ra::filesystem::CopyFileUtf8(current_process_path, new_process_path);
    else
      copied = ra::filesystem::CopyFile(current_process_path, new_process_path);
    if (!copied)
    {
      error_message = "Failed copying file '" + current_process_path + "' to '" + new_process_path + "'.";
      return false;
    }

#ifdef __linux__
    //Set new process as executable
    std::string command;
    command.append("chmod 777 ");
    command.append(new_process_path);
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    if (exit_code != 0)
    {
      error_message = "Failed running command: " << command;
      return false;
    }
#endif //__linux__

    return true;
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
