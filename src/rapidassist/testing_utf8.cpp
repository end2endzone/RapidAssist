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

#include "rapidassist/testing_utf8.h"
#include "rapidassist/unicode.h"
#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/process_utf8.h"

#ifdef _WIN32
#include <Windows.h> //for CreateFile()
#include "rapidassist/undef_windows_macros.h"
#endif

namespace ra { namespace testing {

#ifdef _WIN32 // UTF-8

  extern bool IsFileEquals(FILE* file1, FILE* file2, std::string & reason, size_t max_differences);
  extern bool GetFileDifferences(FILE* file1, FILE* file2, std::vector<FileDiff> & differences, size_t max_differences);

  //
  // Description:
  //  Wrapper class for FILE* instance.
  //  Which automatically closes the FILE* on wrapper's destruction
  // 
  class FileWrapperUtf8 {
  public:
    FileWrapperUtf8(const char * path, const char * mode) {
      std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
      std::wstring modeW = ra::unicode::Utf8ToUnicode(mode);
      file_pointer_ = _wfopen(pathW.c_str(), modeW.c_str());
    }

    ~FileWrapperUtf8() {
      Close();
    }

    bool IsEof() {
      if (file_pointer_ == NULL)
        return true;
      //http://www.cplusplus.com/reference/cstdio/feof/
      return (feof(file_pointer_) != 0);
    }

    void Close() {
      if (file_pointer_) {
        fclose(file_pointer_);
        file_pointer_ = NULL;
      }
    }

    //members
    FILE * file_pointer_;
  };

  bool IsFileEqualsUtf8(const char* file1, const char* file2, std::string & reason, size_t max_differences) {
    //Build basic message
    reason.clear();
    reason << "Comparing first file \"" << file1 << "\" with second file \"" << file2 << "\". ";

    FileWrapperUtf8 f1(file1, "rb");
    if (f1.file_pointer_ == NULL) {
      reason << "First file is not found.";
      return false;
    }
    FileWrapperUtf8 f2(file2, "rb");
    if (f2.file_pointer_ == NULL) {
      reason << "Second file is not found.";
      return false;
    }

    bool result = IsFileEquals(f1.file_pointer_, f2.file_pointer_, reason, max_differences);
    return result;
  }

  bool GetFileDifferencesUtf8(const char* file1, const char* file2, std::vector<FileDiff> & differences, size_t max_differences) {
    FileWrapperUtf8 f1(file1, "rb");
    if (f1.file_pointer_ == NULL)
      return false;
    FileWrapperUtf8 f2(file2, "rb");
    if (f2.file_pointer_ == NULL)
      return false;

    bool result = GetFileDifferences(f1.file_pointer_, f2.file_pointer_, differences, max_differences);
    return result;
  }

  bool FindInFileUtf8(const char* path, const char* value, int & line_index, int & character_index) {
    if (!ra::filesystem::FileExistsUtf8(path))
      return false;

    line_index = -1;
    character_index = -1;

    ra::strings::StringVector lines;
    bool success = ra::filesystem::ReadTextFileUtf8(path, lines);
    if (!success)
      return false;

    for (size_t i = 0; i < lines.size(); i++) {
      const std::string & line = lines[i];
      size_t position = line.find(value, 0);
      if (position != std::string::npos) {
        line_index = (int)i;
        character_index = (int)position;
        return true;
      }
    }

    return false;
  }

  bool CreateFileUtf8(const char * path, size_t size) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE * f = _wfopen(pathW.c_str(), L"wb");
    if (!f)
      return false;

    for (size_t i = 0; i < size; i++) {
      unsigned int value = (i % 256);
      fwrite(&value, 1, 1, f);
    }
    fclose(f);
    return true;
  }

  bool CreateFileUtf8(const char * path) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE * f = _wfopen(pathW.c_str(), L"w");
    if (f == NULL)
      return false;
    fputs("FOO!\n", f);
    fputs("&\n", f);
    fputs("BAR\n", f);
    fclose(f);

    return true;
  }

  bool CreateFileSparseUtf8(const char * path, uint64_t size) {
    //https://stackoverflow.com/questions/982659/quickly-create-large-file-on-a-windows-system

    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);

    LARGE_INTEGER large_integer;
    large_integer.QuadPart = size;

    HANDLE hFile = ::CreateFileW(pathW.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE)
      return false;
    if (SetFilePointerEx(hFile, large_integer, 0, FILE_BEGIN) == 0)
    {
      CloseHandle(hFile);
      return false;
    }
    if (SetEndOfFile(hFile) == 0)
    {
      CloseHandle(hFile);
      return false;
    }
    if (CloseHandle(hFile) == 0)
      return false;
    return true;
  }

  void ChangeFileContentUtf8(const char * path, size_t offset, unsigned char value) {
    //read
    std::wstring pathW = ra::unicode::Utf8ToUnicode(path);
    FILE * f = _wfopen(pathW.c_str(), L"rb");
    if (!f)
      return;

    uint32_t size = ra::filesystem::GetFileSize(f);
    unsigned char * buffer = new unsigned char[size];
    if (!buffer)
      return;
    size_t byte_read = fread(buffer, 1, size, f);
    fclose(f);

    //modify
    if (offset < (size_t)size)
      buffer[offset] = value;

    //save
    f = _wfopen(pathW.c_str(), L"wb");
    if (!f)
      return;
    size_t byte_write = fwrite(buffer, 1, size, f);
    fclose(f);
  }

  bool CloneExecutableFileUtf8(const std::string & target_path, std::string & error_message)
  {
    error_message.clear();

    //Find current executable's path
    std::string current_process_path = ra::process::GetCurrentProcessPathUtf8();
    if (current_process_path.empty() || !ra::filesystem::FileExistsUtf8(current_process_path.c_str()))
    {
      error_message = "Unable to identify current process file path.";
      return false;
    }

    //Copy executable file to target path
    bool copied = ra::filesystem::CopyFileUtf8(current_process_path, target_path);
    if (!copied)
    {
      error_message = "Failed copying file '" + current_process_path + "' to '" + target_path + "'.";
      return false;
    }

#if defined(__linux__) || defined(__APPLE__)
    //On Linux and macOS, the execute flag must be set on the target file
    std::string command;
    command.append("chmod +x ");
    command.append(target_path);
    int system_result = system(command.c_str());
    int exit_code = WEXITSTATUS( system_result );
    if (exit_code != 0)
    {
      error_message = "Failed running command: " + command;
      return false;
    }
#endif //__linux__ || __APPLE__

    return true;
  }

  bool CloneExecutableTempFileUtf8(std::string & output_path, std::string & error_message)
  {
    output_path.clear();
    
    static const std::string separator = ra::filesystem::GetPathSeparatorStr();

    //Get the temporary directory
    std::string temp_dir = ra::filesystem::GetTemporaryDirectoryUtf8();

    //Build a temporary filename with an utf8 character
    std::string temp_path = temp_dir + separator + ra::filesystem::GetTemporaryFileName();
    temp_path.append(".psi_\xCE\xA8_psi");
#ifdef _WIN32
    temp_path.append(".exe");
#endif

    bool cloned = CloneExecutableFileUtf8(temp_path, error_message);
    if (cloned)
      output_path = temp_path;
    
    return cloned;
  }

#endif // UTF-8

} //namespace testing
} //namespace ra
