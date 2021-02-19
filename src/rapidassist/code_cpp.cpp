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

#include "rapidassist/code_cpp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <sstream>

//http://stackoverflow.com/questions/10057258/how-does-x-work-in-a-string
//http://en.cppreference.com/w/cpp/language/escape

namespace ra { namespace code { namespace cpp {

  struct ControlCharacter {
    char c;
    const char * escape_str;
  };
  //https://stackoverflow.com/questions/10220401/rules-for-c-string-literals-escape-character
  static const ControlCharacter kCtrlChars[] = {
    {'\0',"\\0"},   //0x00, null
    {'\a',"\\a"},   //0x07, alert (bell)
    {'\b',"\\b"},   //0x08, backspace
    {'\t',"\\t"},   //0x09, horizontal tab
    {'\n',"\\n"},   //0x0A, newline (or line feed)
    {'\v',"\\v"},   //0x0B, vertical tab
    {'\f',"\\f"},   //0x0C, form feed
    {'\r',"\\r"},   //0x0D, carriage return
    //{'\e',"\\e"}, //0x1B, //escape.  VS2010: warning C4129: 'e' : unrecognized character escape sequence
    {'\"',"\\\""},  //0x22
    {'\'',"\\\'"},  //0x27
    {'\?',"\\\?"},  //0x3F
    {'\\',"\\\\"},  //0x5C
  };
  static const size_t kNumCtrlChars = sizeof(kCtrlChars) / sizeof(kCtrlChars[0]);

  bool IsPrintableCharacter(const char c) {
    if (c == 39) // character ' must be escaped with \' which is not supported right now
      return false;
    if (c == 92) // character \ must be escaped with \\ which is not supported right now
      return false;
    if (c >= 32 && c <= 126)
      return true;
    return false;
  }

  bool IsControlCharacter(char c) {
    for (size_t i = 0; i < kNumCtrlChars; i++) {
      const ControlCharacter & ctrl = kCtrlChars[i];
      if (ctrl.c == c)
        return true;
    }
    return false;
  }

  bool IsHexCharacter(char c) {
    if (c >= '0' && c <= '9')
      return true;
    if (c >= 'a' && c <= 'f')
      return true;
    if (c >= 'A' && c <= 'F')
      return true;
    return false;
  }

  bool IsDigitCharacter(char c) {
    if (c >= '0' && c <= '9')
      return true;
    return false;
  }

  const char * GetControlCharacterEscapeString(char c) {
    for (size_t i = 0; i < kNumCtrlChars; i++) {
      const ControlCharacter & ctrl = kCtrlChars[i];
      if (ctrl.c == c)
        return ctrl.escape_str;
    }
    return NULL;
  }

  const char * ToOctString(unsigned char c) {
    static char buffer[] = { '\\', '9', '9', '9', '\0' };
    static const char * octal_characters = "01234567";
    buffer[3] = octal_characters[c % 8];
    c /= 8;
    buffer[2] = octal_characters[c % 8];
    c /= 8;
    buffer[1] = octal_characters[c];
    return buffer;
  }

  const char * ToHexString(unsigned char c) {
    static char buffer[] = { '\\', 'x', 'f', 'f', '\0' };
    static const char * hexadecimal_characters = "0123456789abcdef";
    buffer[3] = hexadecimal_characters[c % 16];
    c /= 16;
    buffer[2] = hexadecimal_characters[c];
    return buffer;
  }

  std::string ToOctString(const unsigned char * buffer, size_t size) {
    return ToOctString(buffer, size, true);
  }

  std::string ToOctString(const unsigned char * buffer, size_t size, bool disable_warning_c4125) {
    std::string output;

    //estimate the size of the output string to prevent memory copy
    //assume 50% of buffer is *NOT* printable
    size_t non_printable_size = ((size * 50) / 100);
    size_t estimated_string_size = size - non_printable_size + non_printable_size * 4; //4 bytes per octal characters
    output.reserve(estimated_string_size);

    enum CHARACTER_TYPE {
      OCTAL,
      CONTROL,
      PRINTABLE,
    };

    CHARACTER_TYPE previous = PRINTABLE;
    for (size_t i = 0; i < size; i++) {
      unsigned char c = buffer[i];
      unsigned char next = buffer[i + 1];
      if (i + 1 == size) //if out of scope
        next = '\0';

      if (c == 0 && !IsDigitCharacter(next)) {
        //safe to encode NULL character as '\0' instead of '\000'
        output.append(GetControlCharacterEscapeString(c));
        previous = OCTAL;
      }
      else if (c == 0) {
        output.append(ToOctString(c));
        previous = OCTAL;
      }
      else if (IsControlCharacter(c)) {
        output.append(GetControlCharacterEscapeString(c));
        previous = CONTROL;
      }
      else if (disable_warning_c4125 && previous == OCTAL && IsDigitCharacter(c)) //prevent warning C4125: decimal digit terminates octal escape sequence
      {
        //character must be encoded as octal instead of printable
        output.append(ToOctString(c));
        previous = OCTAL;
      }
      else if (IsPrintableCharacter(c)) {
        output.append(1, c);
        previous = PRINTABLE;
      }
      else {
        output.append(ToOctString(c));
        previous = OCTAL;
      }
    }

    return output;
  }

  std::string ToHexString(const unsigned char * buffer, size_t size) {
    std::string output;

    //estimate the size of the output string to prevent memory copy
    //assume 50% of buffer is *NOT* printable
    size_t non_printable_size = ((size * 50) / 100);
    size_t estimated_string_size = size - non_printable_size + non_printable_size * 4; //4 bytes per hex characters
    output.reserve(estimated_string_size);

    enum CHARACTER_TYPE {
      HEX,
      CONTROL,
      PRINTABLE,
    };

    CHARACTER_TYPE previous = PRINTABLE;
    for (size_t i = 0; i < size; i++) {
      unsigned char c = buffer[i];
      unsigned char next = buffer[i + 1];
      if (i + 1 == size) //if out of scope
        next = '\0';

      if (c == 0 && !IsDigitCharacter(next)) {
        //safe to encode NULL character as '\0' instead of '\000'
        output.append(GetControlCharacterEscapeString(c));
        previous = CONTROL;
      }
      else if (c == 0) {
        output.append(ToHexString(c));
        previous = HEX;
      }
      else if (IsControlCharacter(c)) {
        output.append(GetControlCharacterEscapeString(c));
        previous = CONTROL;
      }
      else if (previous == HEX && IsHexCharacter(c)) //an hexadecimal letter cannot follow an hexadecimal escape sequence.
      {
        //must also be printed as an hexadecimal escape sequence
        //http://stackoverflow.com/questions/5784969/when-did-c-compilers-start-considering-more-than-two-hex-digits-in-string-lite
        output.append(ToHexString(c));
        previous = HEX;
      }
      else if (IsPrintableCharacter(c)) {
        output.append(1, c);
        previous = PRINTABLE;
      }
      else {
        output.append(ToHexString(c));
        previous = HEX;
      }
    }

    return output;
  }

  std::string ToCppCharactersArray(const unsigned char * buffer, size_t size) {
    std::ostringstream oss;

    for (size_t i = 0; i < size; i++) {
      unsigned char c = buffer[i];

      if (IsPrintableCharacter((char)c))
        oss << '\'' << (char)c << '\'';
      else
        oss << (int)c; //print as decimal value

      size_t lastByteIndex = size - 1;

      if (i != lastByteIndex)
        oss << ",";
    }

    return oss.str();
  }

} //cpp
} //code
} //namespace ra
