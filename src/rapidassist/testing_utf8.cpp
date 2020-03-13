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

namespace ra { namespace testing {

#ifdef _WIN32 // UTF-8

  extern bool IsFileEquals(FILE* iFile1, FILE* iFile2, std::string & oReason, size_t iMaxDifferences);
  extern bool GetFileDifferences(FILE* iFile1, FILE* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences);

  //
  // Description:
  //  Wrapper class for FILE* instance.
  //  Which automatically closes the FILE* on wrapper's destruction
  // 
  class FileWrapperUtf8 {
  public:
    FileWrapperUtf8(const char * iPath, const char * iMode) {
      std::wstring pathW = ra::unicode::Utf8ToUnicode(iPath);
      std::wstring modeW = ra::unicode::Utf8ToUnicode(iMode);
      file_pointer_ = _wfopen(pathW.c_str(), modeW.c_str());
    }

    ~FileWrapperUtf8() {
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

  bool IsFileEqualsUtf8(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences) {
    //Build basic message
    oReason.clear();
    oReason << "Comparing first file \"" << iFile1 << "\" with second file \"" << iFile2 << "\". ";

    FileWrapperUtf8 f1(iFile1, "rb");
    if (f1.file_pointer_ == NULL) {
      oReason << "First file is not found.";
      return false;
    }
    FileWrapperUtf8 f2(iFile2, "rb");
    if (f2.file_pointer_ == NULL) {
      oReason << "Second file is not found.";
      return false;
    }

    bool result = IsFileEquals(f1.file_pointer_, f2.file_pointer_, oReason, iMaxDifferences);
    return result;
  }

  bool GetFileDifferencesUtf8(const char* iFile1, const char* iFile2, std::vector<FileDiff> & oDifferences, size_t iMaxDifferences) {
    FileWrapperUtf8 f1(iFile1, "rb");
    if (f1.file_pointer_ == NULL)
      return false;
    FileWrapperUtf8 f2(iFile2, "rb");
    if (f2.file_pointer_ == NULL)
      return false;

    bool result = GetFileDifferences(f1.file_pointer_, f2.file_pointer_, oDifferences, iMaxDifferences);
    return result;
  }

  bool FindInFileUtf8(const char* iFilename, const char* iValue, int & oLine, int & oCharacter) {
    if (!ra::filesystem::FileExistsUtf8(iFilename))
      return false;

    oLine = -1;
    oCharacter = -1;

    ra::strings::StringVector lines;
    bool success = ra::filesystem::ReadTextFileUtf8(iFilename, lines);
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

  bool CreateFileUtf8(const char * iFilePath, size_t iSize) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(iFilePath);
    FILE * f = _wfopen(pathW.c_str(), L"wb");
    if (!f)
      return false;

    for (size_t i = 0; i < iSize; i++) {
      unsigned int value = (i % 256);
      fwrite(&value, 1, 1, f);
    }
    fclose(f);
    return true;
  }

  bool CreateFileUtf8(const char * iFilePath) {
    std::wstring pathW = ra::unicode::Utf8ToUnicode(iFilePath);
    FILE * f = _wfopen(pathW.c_str(), L"w");
    if (f == NULL)
      return false;
    fputs("FOO!\n", f);
    fputs("&\n", f);
    fputs("BAR\n", f);
    fclose(f);

    return true;
  }

  void ChangeFileContentUtf8(const char * iFilePath, size_t iOffset, unsigned char iValue) {
    //read
    std::wstring pathW = ra::unicode::Utf8ToUnicode(iFilePath);
    FILE * f = _wfopen(pathW.c_str(), L"rb");
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
    f = _wfopen(pathW.c_str(), L"wb");
    if (!f)
      return;
    size_t byteWrite = fwrite(buffer, 1, size, f);
    fclose(f);
  }

#endif // UTF-8

} //namespace testing
} //namespace ra
