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

#include "rapidassist/config.h"
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
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="reason">A textual reason why the files are not identical. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of textual differences. Use -1 for finding all differences. Use 1 to return as soon as a difference is found.</param>
  /// <returns>Returns true if the content of two files is identical. Returns false otherwise.</returns>
  bool IsFileEquals(const char* file1, const char* file2, std::string & reason, size_t max_differences);

  inline bool IsFileEquals(const char* file1, const char* file2) { std::string reason; return IsFileEquals(file1, file2, reason, 1 /*return ASAP*/); }
  inline bool IsFileEquals(const char* file1, const char* file2, std::string & reason) { return IsFileEquals(file1, file2, reason, 1 /*return ASAP*/); }

  /// <summary>
  /// Returns the location of all differences in two files.
  /// </summary>
  /// <param name="file1">The path of the first  file.</param>
  /// <param name="file2">The path of the second file.</param>
  /// <param name="differences">The output list of all differences within both files. Empty if files are identical.</param>
  /// <param name="max_differences">The maximum number of differences.</param>
  /// <returns>Returns the location of all differences in two files. Returns false otherwise.</returns>
  bool GetFileDifferences(const char* file1, const char* file2, std::vector<FileDiff> & differences, size_t max_differences);

  /// <summary>
  /// Returns true if the given text is found in a file.
  /// </summary>
  /// <param name="path">The path of the search file.</param>
  /// <param name="value">The search value.</param>
  /// <param name="line_index">The line number where value is found.</param>
  /// <param name="character_index">The character offset within the line.</param>
  /// <returns>Returns true if the given text is found in a file. Returns false otherwise.</returns>
  bool FindInFile(const char* path, const char* value, int & line_index, int & character_index);

  /// <summary>
  /// Returns the content (each lines) of a file.
  /// The function is deprecated.
  /// </summary>
  /// <remarks>The function is deprecated.</remarks>
  /// <param name="path">The path of the file.</param>
  /// <param name="lines">The output content of the file line by line.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  DEPRECATED bool GetTextFileContent(const char* path, ra::strings::StringVector & lines);

  /// <summary>
  /// Creates a file of the given size. All bytes are sequential.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size in bytes of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFile(const char * path, size_t size);

  /// <summary>
  /// Creates a text file.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFile(const char * path);

  /// <summary>
  /// Creates a large file.
  /// </summary>
  /// <remarks>
  /// https://en.wikipedia.org/wiki/Sparse_file
  /// https://stackoverflow.com/questions/43126760/what-is-a-sparse-file-and-why-do-we-need-it
  /// </remarks>
  /// <param name="path">The path of the file.</param>
  /// <param name="size">The size of the file, in bytes.</param>
  /// <returns>Returns true on success. Returns false otherwise.</returns>
  bool CreateFileSparse(const char * path, uint64_t size);

  /// <summary>
  /// Modify a given byte with the specified value.
  /// </summary>
  /// <param name="path">The path of the file.</param>
  /// <param name="offset">The offset of the modified byte.</param>
  /// <param name="value">The value of the replacement.</param>
  void ChangeFileContent(const char * path, size_t offset, unsigned char value);

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
  /// <param name="positive_filter">positive filter</param>
  /// <param name="negative_filter">negative filter</param>
  /// <param name="existing_filter">existing filter or a pre-build filter (build from a previous call to MergeFilter)</param>
  /// <returns>The the resulting merged gtest filter.</returns>
  std::string MergeFilter(const std::string & positive_filter, const std::string & negative_filter, const char * existing_filter);
  std::string MergeFilter(const std::string & positive_filter, const std::string & negative_filter, int argc, char **argv);

  inline std::string MergeFilter(const std::string & positive_filter, const std::string & negative_filter) { return MergeFilter(positive_filter, negative_filter, NULL); }

  /// <summary>
  /// This function split a GTEST filter into a positive and a negative filter.
  /// </summary>
  /// <remarks>filter must not be NULL</remarks>
  /// <example>
  /// calling SplitFilter("", ..., ...) returns the "" as positive filter and "" as negative filter
  /// calling SplitFilter("*", ..., ...) returns the "*" as positive filter and "" as negative filter
  /// calling SplitFilter("-TestLib.testCase", ..., ...) returns the "" as positive filter and "TestLib.testCase" as negative filter
  /// calling SplitFilter("TestFoo.testFoo-TestLib.testCase:TestMath.testMath", ..., ...) returns the "TestFoo.testFoo" as positive filter and "TestLib.testCase:TestMath.testMath" as negative filter
  /// </example>
  /// <param name="filter">The gtest filter containing a positive and negative filter.</param>
  /// <param name="positive_filter">The output positive filter in filter.</param>
  /// <param name="negative_filter">The output negative filter in filter.</param>
  void SplitFilter(const char * filter, std::string & positive_filter, std::string & negative_filter);

  /// <summary>
  /// Returns a list of all runnable test cases in a gTest compatible test.
  /// Each test case is listed in the following format: testsuite.testcase
  /// </summary>
  /// <param name="path">Path to the GTest compatible executable file.</param>
  /// <returns>Returns a list of all runnable test cases in a gTest compatible test. Returns an empty string if no test was found.</returns>
  ra::strings::StringVector GetTestList(const char * path);
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

  /// <summary>
  /// Returns true if the specified continuous integration server is running.
  /// </summary>
  /// <returns>Returns true if running on Github. Returns false otherwise.</returns>
  bool IsGitHubActions();

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

  /// <summary>
  /// Copy the file of the current process to another file.
  /// </summary>
  /// <param name="target_path">Target file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  bool CloneExecutableFile(const std::string & target_path, std::string & error_message);

  /// <summary>
  /// Copy the file of the current process to a temporary file.
  /// </summary>
  /// <param name="output_path">Output file path of the duplicated process.</param>
  /// <param name="error_message">A non empty error description message if the function has failed.</param>
  /// <returns>Returns true if duplicate process is successful. Returns false otherwise.</returns>
  bool CloneExecutableTempFile(std::string & output_path, std::string & error_message);

} //namespace testing
} //namespace ra

#endif //RA_TESTING_H
