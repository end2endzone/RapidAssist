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

  /// <summary>
  /// Define a difference between two file.
  /// </summary>
  struct FileDiff {
    size_t offset;    //offset in files where a difference is located
    unsigned char c1; //character of first  file
    unsigned char c2; //character of second file
  };

  /// <summary>
  /// Returns true if the content of two files is identical.
  /// </summary>
  /// <param name="iFile1">The path of the first  file.</param>
  /// <param name="iFile2">The path of the second file.</param>
  /// <param name="oReason">A textual reason why the files are not identical. Empty if files are identical.</param>
  /// <param name="iMaxDifferences">The maximum number of textual differences. Use -1 for finding all differences. Use 1 to return as soon as a difference is found.</param>
  /// <returns>Returns true if the content of two files is identical. Returns false otherwise.</returns>
  bool IsFileEquals(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences);

  inline bool IsFileEquals(const char* iFile1, const char* iFile2) { std::string reason; return IsFileEquals(iFile1, iFile2, reason, 1 /*return ASAP*/); }
  inline bool IsFileEquals(const char* iFile1, const char* iFile2, std::string & oReason) { return IsFileEquals(iFile1, iFile2, oReason, 1 /*return ASAP*/); }

  /// <summary>
  /// Returns the location of all differences in two files.
  /// </summary>
  /// <param name="iFile1">The path of the first  file.</param>
  /// <param name="iFile2">The path of the second file.</param>
  /// <param name="oDifferences">The list of all differences within both files. Empty if files are identical.</param>
  /// <param name="iMaxDifferences">The maximum number of differences.</param>
  /// <returns>Returns the location of all differences in two files. Returns false otherwise.</returns>
  bool GetFileDifferences(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences);

  /// <summary>
  /// Returns true if the given text is found in a file.
  /// </summary>
  /// <param name="iFilename">The path of the search file.</param>
  /// <param name="iValue">The search value.</param>
  /// <param name="oLine">The line number where iValue is found.</param>
  /// <param name="oCharacter">The character offset within the line.</param>
  /// <returns>Returns true if the given text is found in a file. Returns false otherwise.</returns>
  bool FindInFile(const char* iFilename, const char* iValue, int & oLine, int & oCharacter);

  /// <summary>
  /// Returns the content (each lines) of a file.
  /// The function is deprecated.
  /// </summary>
  /// <remarks>The function is deprecated.</remarks>
  /// <param name="iFilename">The path of the file.</param>
  /// <param name="oLines">The content of the file line by line.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  DEPRECATED bool GetTextFileContent(const char* iFilename, ra::strings::StringVector & oLines);

  /// <summary>
  /// Creates a file of the given size. All bytes are sequential.
  /// </summary>
  /// <param name="iFilePath">The path of the file.</param>
  /// <param name="iSize">The size in bytes of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFile(const char * iFilePath, size_t iSize);

  /// <summary>
  /// Creates a text file.
  /// </summary>
  /// <param name="iFilePath">The path of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFile(const char * iFilePath);

  /// <summary>
  /// Creates a large file.
  /// </summary>
  /// <remarks>
  /// https://en.wikipedia.org/wiki/Sparse_file
  /// https://stackoverflow.com/questions/43126760/what-is-a-sparse-file-and-why-do-we-need-it
  /// </remarks>
  /// <param name="iFilePath">The path of the file.</param>
  /// <param name="iSize">The size of the file, in bytes.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFileSparse(const char * iFilePath, uint64_t iSize);

  /// <summary>
  /// Modify a given byte with the specified value.
  /// </summary>
  /// <param name="iFilePath">The path of the file.</param>
  /// <param name="iOffset">The offset of the modified byte.</param>
  /// <param name="iValue">The value of the replacement.</param>
  void ChangeFileContent(const char * iFilePath, size_t iOffset, unsigned char iValue);

#ifdef RAPIDASSIST_HAVE_GTEST
  /// <summary>
  /// This function build a GTEST filter automatically based on a given positive and a negative filter.
  /// The result filter format is the following:
  ///   positive[-negative] where 
  ///       [ ... ] is optionnal
  ///       positive and negative are ':'-separated test case name lists
  /// </summary>
  /// <example>
  /// calling MergeFilter("", "TestSystem.testRangingLong:TestSystem.testRangingFloat") return the filter
  /// "*-TestSystem.testRangingLong:TestSystem.testRangingFloat"
  /// </example>
  /// <param name="iPositiveFilter">positive filter</param>
  /// <param name="iNegativeFilter">negative filter</param>
  /// <param name="iExistingFilter">existing filter or a pre-build filter (build from a previous call to MergeFilter)</param>
  /// <returns>The the resulting merged gtest filter.</returns>
  std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, const char * iExistingFilter);
  std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, int argc, char **argv);

  inline std::string MergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter) { return MergeFilter(iPositiveFilter, iNegativeFilter, NULL); }

  /// <summary>
  /// This function split a GTEST filter into a positive and a negative filter.
  /// </summary>
  /// <remarks>iFilter must not be NULL</remarks>
  /// <example>
  /// calling SplitFilter("", ..., ...) returns the "" as positive filter and "" as negative filter
  /// calling SplitFilter("*", ..., ...) returns the "*" as positive filter and "" as negative filter
  /// calling SplitFilter("-TestLib.testCase", ..., ...) returns the "" as positive filter and "TestLib.testCase" as negative filter
  /// calling SplitFilter("TestFoo.testFoo-TestLib.testCase:TestMath.testMath", ..., ...) returns the "TestFoo.testFoo" as positive filter and "TestLib.testCase:TestMath.testMath" as negative filter
  /// </example>
  /// <param name="iFilter">The gtest filter containing a positive and negative filter.</param>
  /// <param name="oPositiveFilter">The positive filter in iFilter.</param>
  /// <param name="oNegativeFilter">The negative filter in iFilter.</param>
  void SplitFilter(const char * iFilter, std::string & oPositiveFilter, std::string & oNegativeFilter);

  /// <summary>
  /// Returns a list of all runnable test cases in a gTest compatible test.
  /// Each test case is listed in the following format: testsuite.testcase
  /// </summary>
  /// <param name="iTestCasePath">Path to the GTest compatible executable file.</param>
  /// <returns>Returns a list of all runnable test cases in a gTest compatible test. Returns an empty string if no test was found.</returns>
  ra::strings::StringVector GetTestList(const char * iTestCasePath);
#endif //RAPIDASSIST_HAVE_GTEST

  /// <summary>
  /// Returns true if the specified processor flag is enabled.
  /// The functions are deprecated.
  /// </summary>
  /// <remarks>The functions are deprecated.</remarks>
  DEPRECATED bool IsProcessorX86();
  DEPRECATED bool IsProcessorX64();
  DEPRECATED bool IsDebugCode();
  DEPRECATED bool IsReleaseCode();

  /// <summary>
  /// Returns true if the specified continuous integration server is running.
  /// </summary>
  /// <returns>Returns true if running on AppVeyor. Returns false otherwise.</returns>
  bool IsAppVeyor();

  /// <summary>
  /// Returns true if the specified continuous integration server is running.
  /// </summary>
  /// <returns>Returns true if running on Travis-Ci. Returns false otherwise.</returns>
  bool IsTravis();

  /// <summary>
  /// Returns true if the specified continuous integration server is running.
  /// </summary>
  /// <returns>Returns true if running on Jenkins. Returns false otherwise.</returns>
  bool IsJenkins();

#ifdef RAPIDASSIST_HAVE_GTEST
  /// <summary>
  /// Returns the name of the current Test Suite name. ie "TestFooClass"
  /// </summary>
  /// <returns>Returns the name of the current Test Suite name.</returns>
  std::string GetTestSuiteName();

  /// <summary>
  /// Returns the name of the current Test Case name. ie "testConstructor"
  /// </summary>
  /// <returns>Returns the name of the current Test Case name.</returns>
  std::string GetTestCaseName();

  /// <summary>
  /// Returns the fully qualified name of the current test case name. ie "TestFooClass.testConstructor"
  /// </summary>
  /// <returns>Returns the fully qualified name of the current test case name.</returns>
  std::string GetTestQualifiedName();
#endif //RAPIDASSIST_HAVE_GTEST

} //namespace testing
} //namespace ra

#endif //RA_TESTING_H
