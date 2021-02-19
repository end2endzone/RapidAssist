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

#ifndef RA_CODE_CPP_H
#define RA_CODE_CPP_H

#include <string>

#include "rapidassist/config.h"

namespace ra { namespace code { namespace cpp {

  /// <summary>
  /// Returns true if the given character is printable to a console.
  /// </summary>
  /// <param name="c">The input character value</param>
  /// <returns>Returns true if the given character is printable to a console.</returns>
  bool IsPrintableCharacter(const char c);

  /// <summary>
  /// Returns true if the given character is a string literal control character.
  /// A control character must be escaped with \ to be inserted as a literal character.
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns true if the given character is a string literal control character. Returns false otherwise.</returns>
  bool IsControlCharacter(char c);

  /// <summary>
  /// Returns a string literal that contains the control character.
  /// Returns NULL if the given character is not a control character.
  /// See IsControlCharacter() for details.
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns a string literal that contains the control character. Returns NULL if the given character is not a control character.</returns>
  const char * GetControlCharacterEscapeString(char c);

  /// <summary>
  /// Returns true if the given character is an hexadecimal character.
  /// A hexadecimal character is the number '0' to '9' or the letter 'A' to 'F' (including 'a' to 'f')
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns true if the given character is an hexadecimal character. Returns false otherwise.</returns>
  bool IsHexCharacter(char c);

  /// <summary>
  /// Returns true if the given character is a digit character.
  /// A digit character is the number '0' to '9'.
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns true if the given character is a digit character. Returns false otherwise.</returns>
  bool IsDigitCharacter(char c);

  /// <summary>
  /// Returns a string literal that contains an octal escape sequence of the given character.
  /// The returned buffer is private to the function and must be copied to a local variable before the next call to the function.
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns a string literal that contains an octal escape sequence of the given character.</returns>
  const char * ToOctString(unsigned char c);

  /// <summary>
  /// Returns a string literal that contains an hexadecimal escape sequence of the given character.
  /// The returned buffer is private to the function and must be copied to a local variable before the next call to the function.
  /// </summary>
  /// <param name="c">The given character</param>
  /// <returns>Returns a string literal that contains an hexadecimal escape sequence of the given character.</returns>
  const char * ToHexString(unsigned char c);

  /// <summary>
  /// Converts the given buffer to a c++ string literals.
  /// All printable characters are printed in the string as is.
  /// Characters that matches a known escaped sequence are properly escaped.
  /// Other characters are printed using their octal representation.
  /// </summary>
  /// <remarks>For a shorter string literal, see other ToOctString() API with disable_warning_c4125 set to false.</remarks>
  /// <param name="buffer">The input buffer to convert</param>
  /// <param name="size">The size in bytes of the given buffer</param>
  /// <returns>Returns the string literals of the given buffer</returns>
  std::string ToOctString(const unsigned char * buffer, size_t size);

  /// <summary>
  /// Converts the given buffer to a c++ string literals.
  /// All printable characters are printed in the string as is.
  /// Characters that matches a known escaped sequence are properly escaped.
  /// Other characters are printed using their octal representation.
  /// </summary>
  /// <param name="buffer">The input buffer to convert</param>
  /// <param name="size">The size in bytes of the given buffer</param>
  /// <param name="disable_warning_c4125">
  /// Disable generation of code which could generate compilation warning C4125 (decimal digit terminates octal escape sequence)
  /// The following string would generate warning C4125: const char * buffer = "foo\12345bar";
  /// Allowing C4125 warning result in a shorter string literal.
  /// </param>
  /// <returns>Returns the string literals of the given buffer</returns>
  std::string ToOctString(const unsigned char * buffer, size_t size, bool disable_warning_c4125);

  /// <summary>
  /// Converts the given buffer to a c++ string literals.
  /// All printable characters are printed in the string as is.
  /// Characters that matches a known escaped sequence are properly escaped.
  /// Other characters are printed using their hexadecimal representation.
  /// </summary>
  /// <param name="buffer">The input buffer to convert</param>
  /// <param name="size">The size in bytes of the given buffer</param>
  /// <returns>Returns the string literals of the given buffer</returns>
  std::string ToHexString(const unsigned char * buffer, size_t size);

  /// <summary>
  /// Converts the given buffer to an array of characters.
  /// All printable characters are printed in the array as characters (ie 'a').
  /// Other characters are printed using their hexadecimal representation.
  /// </summary>
  /// <param name="buffer">The input buffer to convert</param>
  /// <param name="size">The size in bytes of the given buffer</param>
  /// <returns>Returns a string representating an array of bytes which matches the given buffer</returns>
  std::string ToCppCharactersArray(const unsigned char * buffer, size_t size);

} //namespace cpp
} //namespace code
} //namespace ra

#endif //RA_CODE_CPP_H
