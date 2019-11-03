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

#include <gtest/gtest.h>

#include "rapidassist/filesystem.h"
#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/cli.h"

namespace ra { namespace testing {

  //
  // Description:
  //  Wrapper class for FILE* instance.
  //  Which automatically closes the FILE* on wrapper's destruction
  // 
  class FileWrapper {
  public:
    FileWrapper(const char * iPath, const char * iMode) {
      file_pointer_ = fopen(iPath, iMode);
    }

    ~FileWrapper() {
      close();
    }

    bool isEOF() {
      if (file_pointer_ == NULL)
        return true;
      //http://www.cplusplus.com/reference/cstdio/feof/
      return (feof(file_pointer_) != 0);
    }

    void close() {
      if (file_pointer_) {
        fclose(file_pointer_);
        file_pointer_ = NULL;
      }
    }

    //members
    FILE * file_pointer_;
  };

  std::string subString2(const std::string & iString, size_t iStart, size_t iCount) {
    std::string tmp = iString;
    tmp.substr(iStart, iCount);
    return tmp;
  }

  std::string mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter) {
    return mergeFilter(iPositiveFilter, iNegativeFilter, NULL);
  }

  std::string mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, int argc, char **argv) {
    //find supplied --gtest_filter argument
    std::string gtest_filter;
    bool found = ra::cli::ParseArgument("gtest_filter", gtest_filter, argc, argv);
    if (found)
      return mergeFilter(iPositiveFilter, iNegativeFilter, gtest_filter.c_str());
    return mergeFilter(iPositiveFilter, iNegativeFilter, NULL);
  }

  std::string mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, const char * iExistingFilter) {
    std::string filter;

    std::string positive_filter = iPositiveFilter;
    std::string negative_filter = iNegativeFilter;

    if (iExistingFilter) {
      std::string arg_positive_filter;
      std::string arg_negative_filter;
      splitFilter(iExistingFilter, arg_positive_filter, arg_negative_filter);

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

  void splitFilter(const char * iFilter, std::string & oPositiveFilter, std::string & oNegativeFilter) {
    oPositiveFilter = "";
    oNegativeFilter = "";

    if (iFilter == NULL)
      return;

    std::string filter_string = iFilter;
    if (filter_string == "")
      return;

    StringVector filters;
    ra::strings::split(filters, iFilter, "-");

    if (filters.size() > 2) {
      //something went wrong filter contains multiple '-' character
      //return the first 2 elements either way
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = filters[1].c_str();
    }
    else if (filters.size() == 2) {
      //positive and negative filter found
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = filters[1].c_str();
    }
    else if (filters.size() == 1) {
      //positive only filter found
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = "";
    }
    else {
      //something went wrong
    }
  }

  StringVector getTestList(const char * iTestCasePath) {
    //check that file exists
    if (!ra::filesystem::FileExists(iTestCasePath))
      return StringVector();

    static const std::string log_filename = "gTestHelper.tmp";

    std::string command_line;
    command_line.append("cmd /c \"");
    command_line.append("\"");
    command_line.append(iTestCasePath);
    command_line.append("\"");
    command_line.append(" --gtest_list_tests");
    command_line.append(" 2>NUL");
    command_line.append(" 1>");
    command_line.append("\"");
    command_line.append(log_filename);
    command_line.append("\"");

    //exec
    int return_code = system(command_line.c_str());
    if (return_code != 0)
      return StringVector();

    if (!ra::filesystem::FileExists(log_filename.c_str()))
      return StringVector();

    //load test case list from log filename
    StringVector test_list;
    static const std::string disabled_test_case_header = "  DISABLED_";
    static const std::string disabled_test_suite_header = "DISABLED_";
    std::string test_suite_name;
    std::string test_case_name;
    FILE * f = fopen(log_filename.c_str(), "r");
    if (!f)
      return StringVector();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, f)) {
      std::string line = buffer;
      line.substr(0, line.size() - 1); //remove CRLF at the end of the 
      if (subString2(line, 0, disabled_test_case_header.size()) == disabled_test_case_header) {
        //do nothing
      }
      else if (subString2(line, 0, 2) == "  ") {
        //test case
        std::string full_test_case_name;
        full_test_case_name.append(test_suite_name);
        full_test_case_name.append(subString2(line, 2, 999));
        test_list.push_back(full_test_case_name);
      }
      else {
        //test suite name
        test_suite_name = "";
        if (subString2(line, 0, disabled_test_suite_header.size()) == disabled_test_suite_header) {
          //disabled test suite
        }
        else {
          test_suite_name = line;
        }
      }
    }

    fclose(f);

    //delete log file
    int remove_result = remove(log_filename.c_str());

    //exec
    return_code = system(command_line.c_str());

    return test_list;
  }

  bool isFileEquals(const char* iFile1, const char* iFile2) {
    std::string reason;
    return isFileEquals(iFile1, iFile2, reason, 1 /*return ASAP*/);
  }

  bool isFileEquals(const char* iFile1, const char* iFile2, std::string & oReason) {
    return isFileEquals(iFile1, iFile2, oReason, 1 /*return ASAP*/);
  }

  bool isFileEquals(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences) {
    //Build basic message
    oReason = "";
    std::stringstream ss;
    ss << "Comparing first file \"" << iFile1 << "\" with second file \"" << iFile2 << "\". ";

    FileWrapper f1(iFile1, "rb");
    if (f1.file_pointer_ == NULL) {
      ss << "First file is not found.";
      oReason = ss.str();
      return false;
    }
    FileWrapper f2(iFile2, "rb");
    if (f2.file_pointer_ == NULL) {
      ss << "Second file is not found.";
      oReason = ss.str();
      return false;
    }

    //Compare by size
    long size1 = ra::filesystem::GetFileSize(f1.file_pointer_);
    long size2 = ra::filesystem::GetFileSize(f2.file_pointer_);
    if (size1 != size2) {
      if (size1 < size2)
        ss << "First file is smaller than Second file: " << size1 << " vs " << size2 << ".";
      else
        ss << "First file is bigger than Second file: " << size1 << " vs " << size2 << ".";
      oReason = ss.str();
      return false;
    }

    //Compare content
    f1.close();
    f2.close();
    std::vector<FileDiff> differences;
    bool success = getFileDifferences(iFile1, iFile2, differences, iMaxDifferences + 1); //search 1 more record to differentiate between exactly iMaxDifferences differences and more than iMaxDifferences differences
    if (!success) {
      ss << "Unable to determine if content is identical...";
      oReason = ss.str();
      return false;
    }

    if (differences.size() == 0) {
      //no diffences. Files are identicals
      return true;
    }

    //Build error message from differences
    ss << "Content is different: ";
    for (size_t i = 0; i < differences.size() && i < iMaxDifferences; i++) {
      const FileDiff & d = differences[i];
      if (i >= 1)
        ss << ", ";
      static const int BUFFER_SIZE = 1024;
      char buffer[BUFFER_SIZE];
#ifdef _WIN32
      sprintf(buffer, "{address %Iu(0x%IX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#else
      sprintf(buffer, "{address %zu(0x%zX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#endif
      ss << buffer;
      //ss << "{at offset " << (d.offset) << "(0x" << std::hex << (int)d.offset << ") has 0x" << std::hex << (int)d.c1 << " vs 0x" << std::hex << (int)d.c2 << "}";
    }
    if (differences.size() > iMaxDifferences)
      ss << ", ...";
    oReason = ss.str();
    return false;
  }

  bool getFileDifferences(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences) {
    FileWrapper f1(iFile1, "rb");
    if (f1.file_pointer_ == NULL)
      return false;
    FileWrapper f2(iFile2, "rb");
    if (f2.file_pointer_ == NULL)
      return false;

    //Check by size
    long size1 = ra::filesystem::GetFileSize(f1.file_pointer_);
    long size2 = ra::filesystem::GetFileSize(f2.file_pointer_);
    if (size1 != size2) {
      return false; //unsupported
    }

    //Compare content
    static const size_t BUFFER_SIZE = 1024;
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    size_t offsetRead = 0;

    //while there is data to read in files
    while (!f1.isEOF() && !f2.isEOF()) {
      size_t readSize1 = fread(buffer1, 1, BUFFER_SIZE, f1.file_pointer_);
      size_t readSize2 = fread(buffer2, 1, BUFFER_SIZE, f2.file_pointer_);
      if (readSize1 != readSize2) {
        //this should not happend since both files are identical in length.
        return false; //failed
      }
      bool contentEquals = memcmp(buffer1, buffer2, readSize1) == 0;
      if (!contentEquals) {
        //current buffers are different

        //Find differences and build file diff info.
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
          unsigned char c1 = (unsigned char)buffer1[i];
          unsigned char c2 = (unsigned char)buffer2[i];
          if (c1 != c2) {
            FileDiff d;
            d.offset = offsetRead + i;
            d.c1 = c1;
            d.c2 = c2;
            oDifferences.push_back(d);

            //check max differences found
            if (oDifferences.size() == iMaxDifferences)
              return true;
          }
        }
      }
      offsetRead += readSize1;
    }
    return true;
  }

  bool findInFile(const char* iFilename, const char* iValue, int & oLine, int & oCharacter) {
    if (!ra::filesystem::FileExists(iFilename))
      return false;

    oLine = -1;
    oCharacter = -1;

    StringVector lines;
    bool success = getTextFileContent(iFilename, lines);
    if (!success)
      return false;

    for (size_t i = 0; i < lines.size(); i++) {
      const std::string & line = lines[i];
      size_t position = line.find(iValue, 0);
      if (position != std::string::npos) {
        oLine = (int)i;
        oCharacter = (int)position;
        return true;
      }
    }

    return false;
  }

  bool getTextFileContent(const char* iFilename, StringVector & oLines) {
    if (iFilename == NULL)
      return false;

    std::string path = iFilename;
    bool success = ra::filesystem::ReadTextFile(path, oLines, true);
    return success;
  }

  bool createFile(const char * iFilePath, size_t iSize) {
    FILE * f = fopen(iFilePath, "wb");
    if (!f)
      return false;
    for (size_t i = 0; i < iSize; i++) {
      unsigned int value = (i % 256);
      fwrite(&value, 1, 1, f);
    }
    fclose(f);
    return true;
  }

  bool createFile(const char * iFilePath) {
    FILE * f = fopen(iFilePath, "w");
    if (f == NULL)
      return false;
    fputs("FOO!\n", f);
    fputs("&\n", f);
    fputs("BAR\n", f);
    fclose(f);

    return true;
  }

  void changeFileContent(const char * iFilePath, size_t iOffset, unsigned char iValue) {
    //read
    FILE * f = fopen(iFilePath, "rb");
    if (!f)
      return;
    long size = ra::filesystem::GetFileSize(f);
    unsigned char * buffer = new unsigned char[size];
    if (!buffer)
      return;
    size_t byteRead = fread(buffer, 1, size, f);
    fclose(f);

    //modify
    if (iOffset < (size_t)size)
      buffer[iOffset] = iValue;

    //save
    f = fopen(iFilePath, "wb");
    if (!f)
      return;
    size_t byteWrite = fwrite(buffer, 1, size, f);
    fclose(f);
  }

  bool isProcessorX86() {
    return environment::IsProcess32Bit();
  }

  bool isProcessorX64() {
    return environment::IsProcess64Bit();
  }

  bool isDebugCode() {
    return environment::IsConfigurationDebug();
  }

  bool isReleaseCode() {
    return environment::IsConfigurationRelease();
  }

  bool isAppVeyor() {
    return !environment::GetEnvironmentVariable("APPVEYOR").empty();
  }

  bool isTravis() {
    return !environment::GetEnvironmentVariable("TRAVIS").empty();
  }

  bool isJenkins() {
    return !environment::GetEnvironmentVariable("JENKINS_URL").empty();
  }

  std::string getTestSuiteName() {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    return name;
  }

  std::string getTestCaseName() {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    return name;
  }

  std::string getTestQualifiedName() {
    const char * test_suite_name = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    const char * test_case_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();

    std::string name;
    name.append(test_suite_name);
    name.append(".");
    name.append(test_case_name);

    return name;
  }

} //namespace testing
} //namespace ra
