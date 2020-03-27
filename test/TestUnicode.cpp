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

#include "TestUnicode.h"
#include "rapidassist/unicode.h"

namespace ra { namespace unicode { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestUnicode::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestUnicode::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testIsAscii)
  {
    ASSERT_TRUE ( ra::unicode::IsAscii("foobar") );
    ASSERT_TRUE ( ra::unicode::IsAscii("\b\t\n\r\\\"!/$%?&*()_+abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") );

#ifdef _WIN32    
    ASSERT_FALSE( ra::unicode::IsAscii("école") );    //school in french
#endif
    ASSERT_FALSE( ra::unicode::IsAscii("\202cole") ); //school in french, encoded in Windows CP 1252.
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testIsValidCp1252)
  {
    ASSERT_TRUE ( IsValidCp1252("foobar") );
    ASSERT_TRUE ( IsValidCp1252("\\\b\t\n\r\"")); //control characters
    ASSERT_TRUE ( IsValidCp1252("!/$%?&*()_+"));  //symbols
    ASSERT_TRUE ( IsValidCp1252("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") );
#ifdef _WIN32    
    ASSERT_TRUE ( IsValidCp1252("español") );     //spanish
    ASSERT_TRUE ( IsValidCp1252("école") );       //school in french
#endif
    ASSERT_TRUE ( IsValidCp1252("espa\244ol") );  //spanish, encoded in Windows CP 1252.
    ASSERT_TRUE ( IsValidCp1252("\202cole") );    //school in french, encoded in Windows CP 1252.
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testIsValidIso8859_1)
  {
    ASSERT_TRUE ( IsValidIso8859_1("foobar") );
    ASSERT_TRUE ( IsValidIso8859_1("!/$%?&*()_+"));  //symbols
    ASSERT_TRUE ( IsValidIso8859_1("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") );
#ifdef _WIN32    
    ASSERT_TRUE ( IsValidIso8859_1("español") );     //spanish
    ASSERT_TRUE ( IsValidIso8859_1("école") );       //school in french
#endif
    ASSERT_TRUE ( IsValidIso8859_1("espa\244ol") );  //spanish, encoded in Windows CP 1252.
    ASSERT_FALSE( IsValidIso8859_1("\202cole") );    //school in french, encoded in Windows CP 1252.

    ASSERT_FALSE( IsValidIso8859_1("\\\b\t\n\r\"")); //control characters
    ASSERT_FALSE( IsValidIso8859_1("\x0d\x0a") );    //CRLF, ISO-8859-1 have no control character
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testIsValidUtf8)
  {
    ASSERT_TRUE ( IsValidUtf8("foobar") );
    ASSERT_TRUE ( IsValidUtf8("\\\b\t\n\r\"")); //control characters
    ASSERT_TRUE ( IsValidUtf8("!/$%?&*()_+"));  //symbols
    ASSERT_TRUE ( IsValidUtf8("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") );

    //accent letters from Windows CP1252 encoding
    ASSERT_TRUE ( IsValidUtf8("espa" "\xC3\xB1" "ol") );  //spanish, encoded in UTF-8, U+00F1
    ASSERT_TRUE ( IsValidUtf8("\xC3\xA9" "cole") );       //school in french, encoded in UTF-8

    ASSERT_TRUE ( IsValidUtf8("\x0d\x0a") );    //CRLF
  }
  //--------------------------------------------------------------------------------------------------
#ifdef _WIN32
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testAnsiUnicode)
  {
    const std::string str_ansi = "école"; //school in french

    std::wstring str_unicode = AnsiToUnicode(str_ansi);
    std::string str_converted = UnicodeToAnsi(str_unicode);

    ASSERT_EQ( str_ansi.size(), str_unicode.size() );
    ASSERT_EQ( str_ansi, str_converted );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestUnicode, testUtf8Unicode)
  {
    const std::string str_utf8 = "\303\251cole"; //school in french

    std::wstring str_unicode = Utf8ToUnicode(str_utf8);
    std::string str_converted = UnicodeToUtf8(str_unicode);

    //The size() method returns the number of character, not the numbre of byte.
    //The utf8 string have one more character than the unicode version because the first characters is counted as 2 bytes because std::string is expected 1 byte/characters.
    ASSERT_EQ( str_utf8.size() - 1, str_unicode.size() );  
    
    //unicode string is missing a byte because the first utf8 characters is 2 bytes in utf but a single character in unicode.
    ASSERT_EQ( str_utf8, str_converted );
  }
  //--------------------------------------------------------------------------------------------------
#endif //_WIN32
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
