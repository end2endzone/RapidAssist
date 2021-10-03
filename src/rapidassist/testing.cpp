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

#include "rapidassist/testing.h"

#include <sstream> //for stringstream
#include <iostream> //for std::hex
#include <cstdio> //for remove()

#ifdef RAPIDASSIST_HAVE_GTEST
#include <gtest/gtest.h>
#endif

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/cli.h"
#include "rapidassist/process.h"

#ifdef _WIN32
#include <Windows.h> //for CreateFile()
#include "rapidassist/undef_windows_macros.h"
#endif

#if defined(__APPLE__)
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace ra { namespace testing {

  //predeclarations
  bool IsFileEquals(FILE* file1, FILE* file2, std::string & reason, size_t max_differences);
  bool GetFileDifferences(FILE* file1, FILE* file2, std::vector<FileDiff> & differences, size_t max_differences);

  //
  // Description:
  //  Wrapper class for FILE* instance.
  //  Which automatically closes the FILE* on wrapper's destruction
  // 
  class FileWrapper {
  public:
    FileWrapper(const char * path, const char * mode) {
      file_pointer_ = fopen(path, mode);
    }

    ~FileWrapper() {
      Close();
    }

    static bool IsEof(FILE * f) {
      if (f == NULL)
        return true;
      //http://www.cplusplus.com/reference/cstdio/feof/
      return (feof(f) != 0);
    }
    bool IsEof() {
      return FileWrapper::IsEof(file_pointer_);
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

#ifdef RAPIDASSIST_HAVE_GTEST
  std::string MergeFilter(const std::string & positive_filter, const std::string & negative_filter, int argc, char **argv) {
    //find supplied --gtest_filter argument
    std::string gtest_filter;
    bool found = ra::cli::ParseArgument("gtest_filter", gtest_filter, argc, argv);
    if (found)
      return MergeFilter(positive_filter, negative_filter, gtest_filter.c_str());
    return MergeFilter(positive_filter, negative_filter, NULL);
  }

  std::string MergeFilter(const std::string & positive_filter_old, const std::string & negative_filter_old, const char * existing_filter) {
    std::string filter;

    std::string positive_filter = positive_filter_old;
    std::string negative_filter = negative_filter_old;

    if (existing_filter) {
      std::string arg_positive_filter;
      std::string arg_negative_filter;
      SplitFilter(existing_filter, arg_positive_filter, arg_negative_filter);

      //append argument filters to positive_filter and negative_filter filters
      if (positive_filter == "")
        positive_filter = arg_positive_filter;
      else if (arg_positive_filter == "" || arg_positive_filter == "*") {
        //nothing to add since arg_positive_filter does not specify test cases
      }
      else {
        positive_filter.append(":");
        positive_filter.append(arg_positive_filter);
      }
      if (negative_filter == "")
        negative_filter = arg_negative_filter;
      else if (arg_negative_filter == "" || arg_negative_filter == "*") {
        //nothing to add since arg_negative_filter does not specify test cases
      }
      else {
        negative_filter.append(":");
        negative_filter.append(arg_negative_filter);
      }
    }

    if (positive_filter != "" || negative_filter != "") {
      if (positive_filter == "")
        filter.append("*");
      else
        filter.append(positive_filter);
      if (negative_filter != "") {
        filter.append("-");
        filter.append(negative_filter);
      }
    }

    return filter;
  }

  void SplitFilter(const char * filter, std::string & positive_filter, std::string & negative_filter) {
    positive_filter = "";
    negative_filter = "";

    if (filter == NULL)
      return;

    std::string filter_string = filter;
    if (filter_string == "")
      return;

    ra::strings::StringVector filters;
    ra::strings::Split(filters, filter, "-");

    if (filters.size() > 2) {
      //something went wrong filter contains multiple '-' character
      //return the first 2 elements either way
      positive_filter = filters[0].c_str();
      negative_filter = filters[1].c_str();
    }
    else if (filters.size() == 2) {
      //positive and negative filter found
      positive_filter = filters[0].c_str();
      negative_filter = filters[1].c_str();
    }
    else if (filters.size() == 1) {
      //positive only filter found
      positive_filter = filters[0].c_str();
      negative_filter = "";
    }
    else {
      //something went wrong
    }
  }

  ra::strings::StringVector GetTestList(const char * path) {
    //check that file exists
    if (!ra::filesystem::FileExists(path))
      return ra::strings::StringVector();

    const std::string log_filename = ra::filesystem::GetTemporaryFileName();

    std::string command_line;
#ifdef _WIN32
    command_line.append("cmd /c \"");
    command_line.append("\"");
    command_line.append(path);
    command_line.append("\"");
    command_line.append(" --gtest_list_tests");
    command_line.append(" 2>NUL");
    command_line.append(" 1>");
    command_line.append("\"");
    command_line.append(log_filename);
    command_line.append("\"");
#elif defined(__linux__) || defined(__APPLE__)
    command_line.append("\"");
    command_line.append(path);
    command_line.append("\"");
    command_line.append(" --gtest_list_tests");
    command_line.append(" 2>NUL");
    command_line.append(" 1>");
    command_line.append("\"");
    command_line.append(log_filename);
    command_line.append("\"");
#endif //_WIN32

    //exec
    int return_code = system(command_line.c_str());
#ifdef _WIN32
    int exit_code = system(command_line.c_str());
#elif defined(__linux__) || defined(__APPLE__)
    //Run the new process and log the output
    int system_result = system(command_line.c_str());
    int exit_code = WEXITSTATUS( system_result );
#endif //_WIN32
    if (return_code != 0)
    {
      printf("Failed running command: %s\n", command_line.c_str());
      return ra::strings::StringVector();
    }

    if (!ra::filesystem::FileExists(log_filename.c_str()))
      return ra::strings::StringVector();

    //load test case list from log filename
    ra::strings::StringVector test_list;
    static const std::string disabled_test_case_header = "  DISABLED_";
    static const std::string disabled_test_suite_header = "DISABLED_";
    std::string test_suite_name;
    std::string test_case_name;
    FILE * f = fopen(log_filename.c_str(), "r");
    if (!f)
    {
      ra::filesystem::DeleteFile(log_filename.c_str()); //cleanup
      return ra::strings::StringVector();
    }

    ra::strings::StringVector lines;
    bool success = ra::filesystem::ReadTextFile(log_filename, lines);
    if (!success)
    {
      ra::filesystem::DeleteFile(log_filename.c_str()); //cleanup
      return ra::strings::StringVector();
    }
    
    for(size_t i=0; i<lines.size(); i++)
    {
      const std::string line = lines[i];
      if (line.substr(0, disabled_test_case_header.size()) == disabled_test_case_header) {
        //do nothing
      }
      else if (line.substr(0, 2) == "  ") {
        //test case
        std::string full_test_case_name;
        full_test_case_name.append(test_suite_name);
        full_test_case_name.append(line.substr(2, 999));
        test_list.push_back(full_test_case_name);
      }
      else {
        //test suite name
        test_suite_name = "";
        if (line.substr(0, disabled_test_suite_header.size()) == disabled_test_suite_header) {
          //disabled test suite
        }
        else {
          test_suite_name = line;
        }
      }
    }

    fclose(f);

    //delete log file
    ra::filesystem::DeleteFile(log_filename.c_str()); //cleanup

    return test_list;
  }
#endif //RAPIDASSIST_HAVE_GTEST

  bool IsFileEquals(const char* file1, const char* file2, std::string & reason, size_t max_differences) {
    //Build basic message
    reason.clear();
    reason << "Comparing first file \"" << file1 << "\" with second file \"" << file2 << "\". ";

    FileWrapper f1(file1, "rb");
    if (f1.file_pointer_ == NULL) {
      reason << "First file is not found.";
      return false;
    }
    FileWrapper f2(file2, "rb");
    if (f2.file_pointer_ == NULL) {
      reason << "Second file is not found.";
      return false;
    }

    bool result = IsFileEquals(f1.file_pointer_, f2.file_pointer_, reason, max_differences);
    return result;
  }

  bool IsFileEquals(FILE* file1, FILE* file2, std::string & reason, size_t max_differences) {
    //Compare by size
    uint32_t size1 = ra::filesystem::GetFileSize(file1);
    uint32_t size2 = ra::filesystem::GetFileSize(file2);
    if (size1 != size2) {
      if (size1 < size2)
        reason << "First file is smaller than Second file: " << size1 << " vs " << size2 << ".";
      else
        reason << "First file is bigger than Second file: " << size1 << " vs " << size2 << ".";
      return false;
    }

    //Compare content
    std::vector<FileDiff> differences;
    bool success = GetFileDifferences(file1, file2, differences, max_differences + 1); //search 1 more record to differentiate between exactly max_differences differences and more than max_differences differences
    if (!success) {
      reason << "Unable to determine if content is identical...";
      return false;
    }

    if (differences.size() == 0) {
      //no diffences. Files are identicals
      reason.clear();
      return true;
    }

    //Build error message from differences
    reason << "Content is different: ";
    for (size_t i = 0; i < differences.size() && i < max_differences; i++) {
      const FileDiff & d = differences[i];
      if (i >= 1)
        reason << ", ";
      static const int BUFFER_SIZE = 1024;
      char buffer[BUFFER_SIZE];
#ifdef _WIN32
      sprintf(buffer, "{address %Iu(0x%IX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#else
      sprintf(buffer, "{address %zu(0x%zX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#endif
      reason << buffer;
      //reason << "{at offset " << (d.offset) << "(0x" << std::hex << (int)d.offset << ") has 0x" << std::hex << (int)d.c1 << " vs 0x" << std::hex << (int)d.c2 << "}";
    }
    if (differences.size() > max_differences)
      reason << ", ...";
    return false;
  }

  bool GetFileDifferences(const char* file1, const char* file2, std::vector<FileDiff> & differences, size_t max_differences) {
    FileWrapper f1(file1, "rb");
    if (f1.file_pointer_ == NULL)
      return false;
    FileWrapper f2(file2, "rb");
    if (f2.file_pointer_ == NULL)
      return false;

    bool result = GetFileDifferences(f1.file_pointer_, f2.file_pointer_, differences, max_differences);
    return result;
  }

  bool GetFileDifferences(FILE* file1, FILE* file2, std::vector<FileDiff> & differences, size_t max_differences) {
    //Check by size
    uint32_t size1 = ra::filesystem::GetFileSize(file1);
    uint32_t size2 = ra::filesystem::GetFileSize(file2);
    if (size1 != size2) {
      return false; //unsupported
    }

    //Compare content
    static const size_t BUFFER_SIZE = 1024;
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    size_t offset_read = 0;

    //while there is data to read in files
    while (!FileWrapper::IsEof(file1) && !FileWrapper::IsEof(file2)) {
      size_t read_size1 = fread(buffer1, 1, BUFFER_SIZE, file1);
      size_t read_size2 = fread(buffer2, 1, BUFFER_SIZE, file2);
      if (read_size1 != read_size2) {
        //this should not happend since both files are identical in length.
        return false; //failed
      }
      bool content_equals = memcmp(buffer1, buffer2, read_size1) == 0;
      if (!content_equals) {
        //current buffers are different

        //Find differences and build file diff info.
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
          unsigned char c1 = (unsigned char)buffer1[i];
          unsigned char c2 = (unsigned char)buffer2[i];
          if (c1 != c2) {
            FileDiff d;
            d.offset = offset_read + i;
            d.c1 = c1;
            d.c2 = c2;
            differences.push_back(d);

            //check max differences found
            if (differences.size() == max_differences)
              return true;
          }
        }
      }
      offset_read += read_size1;
    }
    return true;
  }

  bool FindInFile(const char* path, const char* value, int & line_index, int & character_index) {
    if (!ra::filesystem::FileExists(path))
      return false;

    line_index = -1;
    character_index = -1;

    ra::strings::StringVector lines;
    bool success = ra::filesystem::ReadTextFile(path, lines);
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

  bool GetTextFileContent(const char* path, ra::strings::StringVector & lines) {
    if (path == NULL)
      return false;

    bool success = ra::filesystem::ReadTextFile(path, lines, true);
    return success;
  }

  bool CreateFile(const char * path, size_t size) {
    FILE * f = fopen(path, "wb");
    if (!f)
      return false;

    for (size_t i = 0; i < size; i++) {
      unsigned int value = (i % 256);
      fwrite(&value, 1, 1, f);
    }
    fclose(f);
    return true;
  }

  bool CreateFile(const char * path) {
    FILE * f = fopen(path, "w");
    if (f == NULL)
      return false;
    fputs("FOO!\n", f);
    fputs("&\n", f);
    fputs("BAR\n", f);
    fclose(f);

    return true;
  }

  bool CreateFileSparse(const char * path, uint64_t size) {
#ifdef _WIN32
    //https://stackoverflow.com/questions/982659/quickly-create-large-file-on-a-windows-system

    LARGE_INTEGER large_integer;
    large_integer.QuadPart = size;

    HANDLE hFile = ::CreateFileA(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
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
#elif defined(__linux__)
    //https://stackoverflow.com/questions/257844/quickly-create-a-large-file-on-a-linux-system
    //Valid commands are:
    //  truncate -s 10737418240 10Gigfile.img
    //  fallocate -l 10737418240 10Gigfile.img
    //
    // truncate command creates a sparse file. The file does use actual space on the disk.
    // In other words, you can allocate a 100gb file even if the disk only have 1mb of freespace.
    //
    // fallocate commands creates a real file. It requires enough freespace to create a file.
    // It can be used to quickly fill a hard disk to know how a software handle "hardisk full" errors.
    // An error is returned if the file with the expected size cannot be created.

    std::string fallocate_path = ra::filesystem::FindFileFromPaths("fallocate");
    if (fallocate_path.empty())
      return false;

    std::string current_dir = ra::filesystem::GetCurrentDirectory();

    //Prepare command arguments
    ra::strings::StringVector arguments;
    arguments.push_back("-l");
    arguments.push_back(ra::strings::ToString(size));
    arguments.push_back(path);

    //Run the new executable
    ra::process::processid_t pid = ra::process::StartProcess(fallocate_path, current_dir, arguments);
    if (pid == ra::process::INVALID_PROCESS_ID)
      return false;

    //wait for the process to complete
    int exit_code = 0;
    bool wait_ok = ra::process::WaitExit(pid, exit_code);
    if (!wait_ok)
      return false;

    if (exit_code != 0)
      return false;
    return true;
#elif defined(__APPLE__)
    //https://stackoverflow.com/questions/186077/how-do-i-create-a-sparse-file-programmatically-in-c-on-mac-os-x/186098#186098
    //https://stackoverflow.com/questions/2245193/why-does-open-create-my-file-with-the-wrong-permissions/2245212
    int fd = open(path, O_WRONLY|O_TRUNC|O_CREAT, 0666);
    if (fd == 0)
      return false;

    off_t offset = static_cast<off_t>(size);
    
    // If requested size is 0 bytes
    if (offset == 0) {
      close(fd);
      return true;
    }

    // How many bytes should we skip?
    if (offset >= 2) {
      off_t skip_size_request = offset - 1;
      off_t skip_size_actual = lseek(fd, skip_size_request, SEEK_CUR); // Make a hole

      if (skip_size_request != skip_size_actual) {
        close(fd);
        return false;
      }

      offset -= skip_size_request;
    }
    
    // Write last byte
    write(fd, ".", 1);
    offset--;

    close(fd);

    uint64_t actual_size = ra::filesystem::GetFileSize64(path);
    bool success = (actual_size == size);
    return success;
#endif
  }

  void ChangeFileContent(const char * path, size_t offset, unsigned char value) {
    //read
    FILE * f = fopen(path, "rb");
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
    f = fopen(path, "wb");
    if (!f)
      return;
    size_t byte_write = fwrite(buffer, 1, size, f);
    fclose(f);
  }

  bool IsProcessorX86() {
    return environment::IsProcess32Bit();
  }

  bool IsProcessorX64() {
    return environment::IsProcess64Bit();
  }

  bool IsDebugCode() {
    return environment::IsConfigurationDebug();
  }

  bool IsReleaseCode() {
    return environment::IsConfigurationRelease();
  }

  bool IsAppVeyor() {
    return !environment::GetEnvironmentVariable("APPVEYOR").empty();
  }

  bool IsTravis() {
    return !environment::GetEnvironmentVariable("TRAVIS").empty();
  }

  bool IsJenkins() {
    return !environment::GetEnvironmentVariable("JENKINS_URL").empty();
  }

  bool IsGitHubActions() {
    //From https://docs.github.com/en/free-pro-team@latest/actions/reference/environment-variables:
    //  GITHUB_ACTIONS is always set to true when GitHub Actions is running the workflow.
    //  You can use this variable to differentiate when tests are being run locally or by GitHub Actions.
    return !environment::GetEnvironmentVariable("GITHUB_ACTIONS").empty();
  }

#ifdef RAPIDASSIST_HAVE_GTEST
  std::string GetTestSuiteName() {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    return name;
  }

  std::string GetTestCaseName() {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    return name;
  }

  std::string GetTestQualifiedName() {
    const char * test_suite_name = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    const char * test_case_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();

    std::string name;
    name.append(test_suite_name);
    name.append(".");
    name.append(test_case_name);

    return name;
  }
#endif //RAPIDASSIST_HAVE_GTEST

  bool CloneExecutableFile(const std::string & target_path, std::string & error_message)
  {
    error_message.clear();

    //Find current executable's path
    std::string current_process_path = ra::process::GetCurrentProcessPath();
    if (current_process_path.empty() || !ra::filesystem::FileExists(current_process_path.c_str()))
    {
      error_message = "Unable to identify current process file path.";
      return false;
    }

    //Copy executable file to target path
    bool copied = ra::filesystem::CopyFile(current_process_path, target_path);
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

  bool CloneExecutableTempFile(std::string & output_path, std::string & error_message)
  {
    output_path.clear();
    
    //Build a temporary filename
    std::string temp_path = ra::filesystem::GetTemporaryFilePath();

#ifdef _WIN32
    //Executables ends with the .exe file extension
    temp_path.append(".exe");
#endif

    bool cloned = CloneExecutableFile(temp_path, error_message);
    if (cloned)
      output_path = temp_path;
    
    return cloned;
  }

} //namespace testing
} //namespace ra
