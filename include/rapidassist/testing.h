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

#ifndef RA_TESTING_H
#define RA_TESTING_H

#include <string>
#include <vector>

#include "rapidassist/macros.h"
#include "rapidassist/strings.h"

namespace ra { namespace testing {

  struct FileDiff {
    size_t offset;    //offset in files where a difference is located
    unsigned char c1; //character of first  file
    unsigned char c2; //character of second file
  };

  //
  // Description:
  //  Returns true if the content of two files is identical.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oReason:         A textual reason why the files are not identical. Empty if files are identical.
  //   iMaxDifferences: The maximum number of textual differences.
  // 
  bool IsFileEquals(const char* iFile1, const char* iFile2);
  bool IsFileEquals(const char* iFile1, const char* iFile2, std::string & oReason);
  bool IsFileEquals(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences);

  //
  // Description:
  //  Returns the location of all differences in two files.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oDifferences:    The list of all differences within both files. Empty if files are identical.
  //   iMaxDifferences: The maximum number of differences.
  // 
  bool GetFileDifferences(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences);

  //
  // Description:
  //  Returns true if the given text is found in a file.
  // Arguments:
  //   iFilename:   The path of the search file.
  //   iValue:      The search value.
  //   oLine:       The line number where iValue is found.
  //   oCharacter:  The character offset within the line.
  // 
  bool FindInFile(const char* iFilename, const char* iValue, int & oLine, int & oCharacter);

  //
  // Description:
  //  Returns the content (each lines) of a file.
  // Arguments:
  //   iFilename:   The path of the file.
  //   oLines:      The content of the file line by line.
  // 
  DEPRECATED bool GetTextFileContent(const char* iFilename, ra::strings::StringVector & oLines);

  //
  // Description:
  //  Creates a file of the given size. All bytes are sequential.
  // Arguments:
  //   iFilePath:   The path of the file.
  //   iSize:       The size in bytes of the file.
  // 
  bool CreateFile(const char * iFilePath, size_t iSize);

  //
  // Description:
  //  Creates text file.
  // Arguments:
  //   iFilePath:   The path of the file.
  // 
  bool CreateFile(const char * iFilePath);

  //
  // Description:
  //  Modify a given byte with the specified value.
  // Arguments:
  //   iFilePath: The path of the file.
  //   iOffset:   The offset of the modified byte.
  //   iValue:    The value of the replacement.
  // 
  void ChangeFileContent(const char * iFilePath, size_t iOffset, unsigned char iValue);

  //
  // Description:
  //  This function build a GTEST filter automatically based on a given positive and a negative filter.
  //  The result filter format is the following:
  //    positive[-negative] where 
  //        [ ... ] is optionnal
  //        positive and negative are ':'-separated test case name lists
  // Arguments:
  //   iPositiveFilter:   positive filter
  //   iNegativeFilter:   negative filter
  //   iExistingFilter:   existing filter or a pre-build filter (build from a previous call to MergeFilter)
  // Example:
  //   calling MergeFilter("", "C21XB1.testSniperA2GRanging25:C21XB1.testSniperA2GRanging40") return the filter
  //   "*-C21XB1.testSniperA2GRanging25:C21XB1.testSniperA2GRanging40"
  // 
  std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter);
  std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, const char * iExistingFilter);
  std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, int argc, char **argv);

  //
  // Description:
  //  This function split a GTEST filter into a positive and a negative filter.
  // Notes:
  //   iFilter must not be NULL
  // Example:
  //   calling SplitFilter("", ..., ...) returns the "" as positive filter and "" as negative filter
  //   calling SplitFilter("*", ..., ...) returns the "*" as positive filter and "" as negative filter
  //   calling SplitFilter("-TestLib.testCase", ..., ...) returns the "" as positive filter and "TestLib.testCase" as negative filter
  //   calling SplitFilter("TestFoo.testFoo-TestLib.testCase:TestMath.testMath", ..., ...) returns the "TestFoo.testFoo" as positive filter and "TestLib.testCase:TestMath.testMath" as negative filter
  // 
  void SplitFilter(const char * iFilter, std::string & oPositiveFilter, std::string & oNegativeFilter);

  //
  // Description
  //  Returns a list of all runnable test cases in a gTest compatible test.
  //  Each test case is listed in the following format: testsuite.testcase
  // Example:
  //  TestTesting.testFilters
  // Arguments:
  //  iTestCasePath:    Path to the GTest compatible executable file
  //
  ra::strings::StringVector GetTestList(const char * iTestCasePath);

  //
  // Description
  //  Returns true if the specified processor flag is enabled
  //
  bool IsProcessorX86();
  bool IsProcessorX64();
  bool IsDebugCode();
  bool IsReleaseCode();

  //
  // Description
  //  Returns true if the specified continuous integration server is running.
  //
  bool IsAppVeyor();
  bool IsTravis();
  bool IsJenkins();

  //
  // Description
  //  Returns the name of the current Test Suite name. ie "TestFooClass"
  //
  std::string GetTestSuiteName();

  //
  // Description
  //  Returns the name of the current Test Case name. ie "testConstructor"
  //
  std::string GetTestCaseName();

  //
  // Description
  //  Returns the fully qualified name of the current test case name. ie "TestFooClass.testConstructor"
  //
  std::string GetTestQualifiedName();

} //namespace testing
} //namespace ra

#endif //RA_TESTING_H
