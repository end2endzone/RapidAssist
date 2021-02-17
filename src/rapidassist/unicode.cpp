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

#include "rapidassist/unicode.h"

#ifdef _WIN32
#include <Windows.h>
#include "rapidassist/undef_windows_macros.h"
#endif

namespace ra { namespace unicode {

  static const std::string  EMPTY_STRING;
  static const std::wstring EMPTY_WIDE_STRING;

  bool IsAscii(const char * str)
  {
    const unsigned char * unsigned_str = (const unsigned char *)str;
    //printf("IsAscii(%s)\n", str);
    int offset = 0;
    while (str[offset] != '\0')
    {
      //printf("str[%d]=%d\n", offset, (int)unsigned_str[offset]);
      if (unsigned_str[offset] > 127) //if bit7 is set
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool IsValidCp1252(const char * str)
  {
    const unsigned char * unsigned_str = (const unsigned char *)str;
    //printf("IsValidCp1252(%s)\n", str);
    int offset = 0;
    while (str[offset] != '\0')
    {
      //printf("str[%d]=%d\n", offset, (int)unsigned_str[offset]);
      const unsigned char & c = unsigned_str[offset];
      if (
        c == 0x81 ||
        c == 0x8D ||
        c == 0x8F ||
        c == 0x90 ||
        c == 0x9D )
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool IsValidIso8859_1(const char * str)
  {
    const unsigned char * unsigned_str = (const unsigned char *)str;
    //printf("IsValidIso8859_1(%s)\n", str);
    int offset = 0;
    while (str[offset] != '\0')
    {
      //printf("str[%d]=%d\n", offset, (int)unsigned_str[offset]);
      const unsigned char & c = unsigned_str[offset];
      if (0x00 <= c && c <= 0x1F)
        return false;
      if (0x7F <= c && c <= 0x9F)
        return false;

      //next byte
      offset++;
    }
    return true;
  }

  bool IsValidUtf8(const char * str)
  {
    const unsigned char * unsigned_str = (const unsigned char *)str;
    //printf("IsValidUtf8(%s)\n", str);
    int offset = 0;
    while (str[offset] != '\0')
    {
      //printf("str[%d]=%d\n", offset, (int)unsigned_str[offset]);
      const unsigned char & c1 = unsigned_str[offset + 0];
      unsigned char c2 = unsigned_str[offset + 1];
      unsigned char c3 = unsigned_str[offset + 2];
      unsigned char c4 = unsigned_str[offset + 3];
    
      //prevent going outside of the string
      if (c1 == '\0')
        c2 = c3 = c4 = '\0';
      else if (c2 == '\0')
        c3 = c4 = '\0';
      else if (c3 == '\0')
        c4 = '\0';

      //size in bytes of the code point
      int n = 1;

      //See http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf, Table 3-7. Well-Formed UTF-8 Byte Sequences
      // ## | Code Points         | First Byte | Second Byte | Third Byte | Fourth Byte
      // #1 | U+0000   - U+007F   | 00 - 7F    |             |            | 
      // #2 | U+0080   - U+07FF   | C2 - DF    | 80 - BF     |            | 
      // #3 | U+0800   - U+0FFF   | E0         | A0 - BF     | 80 - BF    | 
      // #4 | U+1000   - U+CFFF   | E1 - EC    | 80 - BF     | 80 - BF    | 
      // #5 | U+D000   - U+D7FF   | ED         | 80 - 9F     | 80 - BF    | 
      // #6 | U+E000   - U+FFFF   | EE - EF    | 80 - BF     | 80 - BF    | 
      // #7 | U+10000  - U+3FFFF  | F0         | 90 - BF     | 80 - BF    | 80 - BF
      // #8 | U+40000  - U+FFFFF  | F1 - F3    | 80 - BF     | 80 - BF    | 80 - BF
      // #9 | U+100000 - U+10FFFF | F4         | 80 - 8F     | 80 - BF    | 80 - BF

      if (c1 <= 0x7F) // #1 | U+0000   - U+007F, (ASCII)
        n = 1;
      else if ( 0xC2 <= c1 && c1 <= 0xDF &&
                0x80 <= c2 && c2 <= 0xBF)  // #2 | U+0080   - U+07FF
        n = 2;
      else if ( 0xE0 == c1 &&
                0xA0 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #3 | U+0800   - U+0FFF
        n = 3;
      else if ( 0xE1 <= c1 && c1 <= 0xEC &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #4 | U+1000   - U+CFFF
        n = 3;
      else if ( 0xED == c1 &&
                0x80 <= c2 && c2 <= 0x9F &&
                0x80 <= c3 && c3 <= 0xBF)  // #5 | U+D000   - U+D7FF
        n = 3;
      else if ( 0xEE <= c1 && c1 <= 0xEF &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF)  // #6 | U+E000   - U+FFFF
        n = 3;
      else if ( 0xF0 == c1 &&
                0x90 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
        n = 4;
      else if ( 0xF1 <= c1 && c1 <= 0xF3 &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #8 | U+40000  - U+FFFFF
        n = 4;
      else if ( 0xF4 == c1 &&
                0x80 <= c2 && c2 <= 0xBF &&
                0x80 <= c3 && c3 <= 0xBF &&
                0x80 <= c4 && c4 <= 0xBF)  // #7 | U+10000  - U+3FFFF
        n = 4;
      else
        return false; // invalid UTF-8 sequence

      //next code point
      offset += n;
    }
    return true;
  }



//--------------------------------------------------------------------------------------------------
#ifdef _WIN32
//--------------------------------------------------------------------------------------------------

  // Convert a wide Unicode string to an UTF8 string
  std::string UnicodeToUtf8(const std::wstring & wstr)
  {
    if (wstr.empty()) return std::string();
    int num_characters = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    if (num_characters == 0)
      return EMPTY_STRING;
    std::string output(num_characters, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &output[0], num_characters, NULL, NULL);
    return output;
  }

  // Convert an UTF8 string to a wide Unicode String
  std::wstring Utf8ToUnicode(const std::string & str)
  {
    if (str.empty()) return std::wstring();
    int num_characters = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    if (num_characters == 0)
      return EMPTY_WIDE_STRING;
    std::wstring output(num_characters, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &output[0], num_characters);
    return output;
  }

  // Convert an wide Unicode string to ANSI string
  std::string UnicodeToAnsi(const std::wstring & wstr)
  {
    if (wstr.empty()) return std::string();
    int num_characters = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    if (num_characters == 0)
      return EMPTY_STRING;
    std::string output(num_characters, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &output[0], num_characters, NULL, NULL);
    return output;
  }

  // Convert an ANSI string to a wide Unicode String
  std::wstring AnsiToUnicode(const std::string & str)
  {
    if (str.empty()) return std::wstring();
    int num_characters = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    if (num_characters == 0)
      return EMPTY_WIDE_STRING;
    std::wstring output(num_characters, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &output[0], num_characters);
    return output;
  }

  std::string Utf8ToAnsi(const std::string & str)
  {
    std::wstring str_unicode = Utf8ToUnicode(str);
    std::string str_ansi = UnicodeToAnsi(str_unicode);
    return str_ansi;
  }

  std::string AnsiToUtf8(const std::string & str)
  {
    std::wstring str_unicode = AnsiToUnicode(str);
    std::string str_utf8 = UnicodeToUtf8(str_unicode);
    return str_utf8;
  }

//--------------------------------------------------------------------------------------------------
#endif //_WIN32
//--------------------------------------------------------------------------------------------------

} //namespace unicode
} //namespace ra
