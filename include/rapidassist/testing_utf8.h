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

#ifndef RA_TESTING_UTF8_H
#define RA_TESTING_UTF8_H

#include <stdint.h>

#include "rapidassist/testing.h"

namespace ra { namespace testing {

#ifdef _WIN32

  //
  // Description:
  //  Returns true if the content of two files is identical.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oReason:         A textual reason why the files are not identical. Empty if files are identical.
  //   iMaxDifferences: The maximum number of textual differences.
  // 
  bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences);

  inline bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2) { std::string reason; return IsFileEqualsUtf8(iFile1, iFile2, reason, -1); }
  inline bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2, std::string & oReason) { return IsFileEqualsUtf8(iFile1, iFile2, oReason, -1); }

  //
  // Description:
  //  Returns the location of all differences in two files.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oDifferences:    The list of all differences within both files. Empty if files are identical.
  //   iMaxDifferences: The maximum number of differences.
  // 
  bool GetFileDifferencesUtf8(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences);

  //
  // Description:
  //  Returns true if the given text is found in a file.
  // Arguments:
  //   iFilename:   The path of the search file.
  //   iValue:      The search value.
  //   oLine:       The line number where iValue is found.
  //   oCharacter:  The character offset within the line.
  // 
  bool FindInFileUtf8(const char* iFilename, const char* iValue, int & oLine, int & oCharacter);

  //
  // Description:
  //  Creates a file of the given size. All bytes are sequential.
  // Arguments:
  //   iFilePath:   The path of the file.
  //   iSize:       The size in bytes of the file.
  // 
  bool CreateFileUtf8(const char * iFilePath, size_t iSize);

  //
  // Description:
  //  Creates text file.
  // Arguments:
  //   iFilePath:   The path of the file.
  // 
  bool CreateFileUtf8(const char * iFilePath);

  //
  // Description:
  //  Modify a given byte with the specified value.
  // Arguments:
  //   iFilePath: The path of the file.
  //   iOffset:   The offset of the modified byte.
  //   iValue:    The value of the replacement.
  // 
  void ChangeFileContentUtf8(const char * iFilePath, size_t iOffset, unsigned char iValue);

#elif __linux__

  //
  // Description:
  //  Returns true if the content of two files is identical.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oReason:         A textual reason why the files are not identical. Empty if files are identical.
  //   iMaxDifferences: The maximum number of textual differences.
  // 
  inline bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2) { return IsFileEquals(iFile1, iFile2); }
  inline bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2, std::string & oReason) { return IsFileEquals(iFile1, iFile2, oReason); }
  inline bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences) { return IsFileEquals(iFile1, iFile2, oReason, iMaxDifferences); }

  //
  // Description:
  //  Returns the location of all differences in two files.
  // Arguments:
  //   iFile1:          The path of the first  file.
  //   iFile2:          The path of the second file.
  //   oDifferences:    The list of all differences within both files. Empty if files are identical.
  //   iMaxDifferences: The maximum number of differences.
  // 
  inline bool GetFileDifferencesUtf8(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences) { return GetFileDifferences(iFile1, iFile2, oDifferences, iMaxDifferences); }

  //
  // Description:
  //  Returns true if the given text is found in a file.
  // Arguments:
  //   iFilename:   The path of the search file.
  //   iValue:      The search value.
  //   oLine:       The line number where iValue is found.
  //   oCharacter:  The character offset within the line.
  // 
  inline bool FindInFileUtf8(const char* iFilename, const char* iValue, int & oLine, int & oCharacter) { return FindInFile(iFilename, iValue, oLine, oCharacter); }

  //
  // Description:
  //  Creates a file of the given size. All bytes are sequential.
  // Arguments:
  //   iFilePath:   The path of the file.
  //   iSize:       The size in bytes of the file.
  // 
  inline bool CreateFileUtf8(const char * iFilePath, size_t iSize) { return CreateFile(iFilePath, iSize); }

  //
  // Description:
  //  Creates text file.
  // Arguments:
  //   iFilePath:   The path of the file.
  // 
  inline bool CreateFileUtf8(const char * iFilePath) { return CreateFile(iFilePath); }

  //
  // Description:
  //  Modify a given byte with the specified value.
  // Arguments:
  //   iFilePath: The path of the file.
  //   iOffset:   The offset of the modified byte.
  //   iValue:    The value of the replacement.
  // 
  inline void ChangeFileContentUtf8(const char * iFilePath, size_t iOffset, unsigned char iValue) { ChangeFileContent(iFilePath, iOffset, iValue); }


#endif

} //namespace testing
} //namespace ra

#endif //RA_TESTING_UTF8_H
