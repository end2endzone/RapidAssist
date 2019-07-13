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

#ifndef RA_STRING_H
#define RA_STRING_H

#include <stdint.h>
#include <string>
#include <vector>
#include <stdio.h>

namespace ra
{
  namespace strings
  {
    typedef std::vector<std::string> StringVector;

    /// <summary>The required amount of precision to get a lossless conversion from  float to string.</summary>
    extern const int  FLOAT_TOSTRING_LOSSLESS_PRECISION;

    /// <summary>The required amount of precision to get a lossless conversion from double to string.</summary>
    extern const int DOUBLE_TOSTRING_LOSSLESS_PRECISION;

    /// <summary>The default epsilon value for converting a  float to string with a minimal lossy conversion.</summary>
    extern const  float  FLOAT_TOSTRING_LOSSY_EPSILON;

    /// <summary>The default epsilon value for converting a double to string with a minimal lossy conversion.</summary>
    extern const double DOUBLE_TOSTRING_LOSSY_EPSILON;

    /// <summary>
    /// Defines if a string value is a numeric value.
    /// A numeric value can be positive or negative.
    /// A numeric value can be an integer or a floating point value.
    /// </summary>
    /// <returns>True when iValue is numeric. False otherwise.</returns>
    bool isNumeric(const char * iValue);

    /// <summary>
    /// Replace an occurance of a string by another.
    /// </summary>
    /// <param name="iString">The given string that need to be searched.</param>
    /// <param name="iOldValue">The old value to replace.</param>
    /// <param name="iNewValue">The new value to replace.</param>
    /// <returns>Returns the number of token that was replaced.</returns>
    int replace(std::string & iString, const std::string & iOldValue, const std::string & iNewValue);

    /// <summary>
    /// Converts the given value to string.
    /// </summary>
    /// <param name="value">The boolean value.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toString(const bool & value);

    /// <summary>
    /// Converts the given value to string.
    /// </summary>
    /// <param name="value">The numeric value.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toString(const   int8_t & value);
    std::string toString(const  uint8_t & value);
    std::string toString(const  int16_t & value);
    std::string toString(const uint16_t & value);
    std::string toString(const  int32_t & value);
    std::string toString(const uint32_t & value);
    std::string toString(const  int64_t & value);
    std::string toString(const uint64_t & value);

    /// <summary>
    /// Converts the given value to string. The conversion to string is lossless. That is no data is lost if the string is converted back to floating point.
    /// </summary>
    /// <remarks>
    /// The string conversion uses as much digits as required so that converting the value to string and back to the original type matches the original value (bitwise compare).
    /// The function assumes that most use of 32 bit floating point values probably target a practical range of   -1 million to   +1 million, and requires a precision of  9.
    /// The function assumes that most use of 64 bit floating point values probably target a practical range of -100 billion to +100 billion, and requires a precision of 17.
    /// However, this creates artifacts. For instance, converting 5.3f to string returns "5.30000019".
    /// If you need digits precision, use toStringFormatted(value, digits) instead.
    /// </remarks>
    /// <param name="value">The numeric value.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toStringLossless(const    float & value);
    std::string toStringLossless(const   double & value);

    /// <summary>
    /// Converts the given value to string.
    /// The conversion to string is lossy. That is data might be lost if the string is converted back to floating point.
    /// However, the result string is the shortest string representation without changing the given value more than epsilon.
    /// The appropriate number of digits after the decimal point is automatically detected.
    /// </summary>
    /// <remarks>
    /// This method of conversion is usually prefered as it creates a more human-friendly (readable) value.
    /// For instance, the value 5.3f which converts to "5.30000019" when lossless has a more meaningful value if read as "5.3".
    /// This is what is intended by this function.
    /// </remarks>
    /// <param name="value">The numeric value.</param>
    /// <param name="epsilon">The amount of acceptable data loss while converting in order to get a smaller value more readable.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toStringLossy(const    float & value, const  float & epsilon);
    std::string toStringLossy(const   double & value, const double & epsilon);

    /// <summary>
    /// Converts the given value to string with a fixed number of digits after the decimal point.
    /// The conversion to string is lossy. That is data might be lost if the string is converted back to floating point.
    /// </summary>
    /// <remarks>
    /// The function is usefull when handling values that have 1/10th or 1/100th increments.
    /// For instance, currency.
    /// </remarks>
    /// <param name="value">The numeric value.</param>
    /// <param name="digits">The number of digits after the decimal point.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toStringFormatted(const    float & value, int digits);
    std::string toStringFormatted(const   double & value, int digits);

    /// <summary>
    /// Converts the given value to string.
    /// </summary>
    /// <remarks>
    /// Note that implementation of this method could change which would affect the quality or the returned value of this function.
    /// To get a constant conversion, one should use toStringLossless(), toStringLossy() or toStringFormatted().
    /// </remarks>
    /// <param name="value">The numeric value.</param>
    /// <returns>Converts the given value to string.</returns>
    std::string toString(const    float & value);
    std::string toString(const   double & value);

    /// <summary>
    /// Parse the given string as a boolean value.
    /// </summary>
    /// <param name="str">The input string which contains a boolean string representation.</param>
    /// <returns>Returns true when the given string is logically 'true'. Returns false otherwise.</returns>
    bool parseBoolean(const std::string & str);

    /// <summary>
    /// Parse the given string into the given numeric variable.
    /// </summary>
    /// <param name="str">The input string which contains a numeric value.</param>
    /// <param name="oValue">The output numeric value.</param>
    /// <returns>Returns true when the parsing is successful.</returns>
    bool parse(const std::string& str,   int8_t & oValue);
    bool parse(const std::string& str,  uint8_t & oValue);
    bool parse(const std::string& str,  int16_t & oValue);
    bool parse(const std::string& str, uint16_t & oValue);
    bool parse(const std::string& str,  int32_t & oValue);
    bool parse(const std::string& str, uint32_t & oValue);
    bool parse(const std::string& str,  int64_t & oValue);
    bool parse(const std::string& str, uint64_t & oValue);
    bool parse(const std::string& str,    float & oValue);
    bool parse(const std::string& str,   double & oValue);
    bool parse(const std::string& str,     bool & oValue);

    /// <summary>
    /// Capitalize the first character of the given string.
    /// </summary>
    /// <param name="iValue">The string value to uppercase the first character.</param>
    /// <returns>Returns the given string with the first character capitalized.</returns>
    std::string capitalizeFirstCharacter(const std::string & iValue);

    /// <summary>
    /// Upper case all characters of the given string.
    /// </summary>
    /// <param name="iValue">The string value to uppercase.</param>
    /// <returns>Returns the given string uppercased.</returns>
    std::string uppercase(const std::string & iValue);

    /// <summary>
    /// Lower case all characters of the given string.
    /// </summary>
    /// <param name="iValue">The string value to lowercase.</param>
    /// <returns>Returns the given string lowercased.</returns>
    std::string lowercase(const std::string & iValue);

    /// <summary>
    /// Removes occurance of unix/windows LF, CR or CRLF into the given string.
    /// </summary>
    /// <param name="iBuffer">The given buffer to modify.</param>
    void removeEOL(char * iBuffer);

    /// <summary>
    /// Splits an input string into multiple string based on the given splitting character.
    /// </summary>
    /// <param name="iText">The input text to split.</param>
    /// <param name="iSplitCharacter">The splitting character.</param>
    /// <returns>Returns a list of string.</returns>
    StringVector split(const std::string & iText, char iSplitCharacter);

    /// <summary>
    /// Splits an input string into multiple string based on the given splitting character.
    /// </summary>
    /// <param name="iText">The input text to split.</param>
    /// <param name="iSplitPattern">The splitting pattern.</param>
    /// <returns>Returns a list of string.</returns>
    StringVector split(const std::string & iText, const char * iSplitPattern);

    /// <summary>
    /// Splits an input string into multiple string based on the given splitting character and store the result in oList.
    /// </summary>
    /// <param name="oList">The output list of string.</param>
    /// <param name="iText">The input text to split.</param>
    /// <param name="iSplitCharacter">The splitting character.</param>
    void split(StringVector & oList, const std::string & iText, char iSplitCharacter);

    /// <summary>
    /// Splits an input string into multiple string based on the given splitting character and store the result in oList.
    /// </summary>
    /// <param name="oList">The output list of string.</param>
    /// <param name="iText">The input text to split.</param>
    /// <param name="iSplitPattern">The splitting pattern.</param>
    void split(StringVector & oList, const std::string & iText, const char * iSplitPattern);

    /// <summary>
    /// Join a list of strings into a single string separating each element by iSeparator.
    /// </summary>
    /// <param name="iList">The list of elements to join.</param>
    /// <param name="iSeparator">The string separator.</param>
    /// <returns>Returns a list of strings merged into a single string separating each element by iSeparator.</returns>
    std::string join(const StringVector & iList, const char * iSeparator);

    /// <summary>
    /// Removes left and right space characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trim(const std::string & iStr);

    /// <summary>
    /// Removes left and right occurrence of iChar characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <param name="iChar">The character to remove.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trim(const std::string & iStr, const char iChar);

    /// <summary>
    /// Removes right space characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trimRight(const std::string & iStr);

    /// <summary>
    /// Removes left space characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trimLeft(const std::string & iStr);

    /// <summary>
    /// Removes right occurrence of iChar characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <param name="iChar">The character to remove.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trimRight(const std::string & iStr, const char iChar);

    /// <summary>
    /// Removes left occurrence of iChar characters of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <param name="iChar">The character to remove.</param>
    /// <returns>Returns the trimmed string.</returns>
    std::string trimLeft(const std::string & iStr, const char iChar);

    /// <summary>
    /// Reverse order each character of the given string.
    /// </summary>
    /// <param name="iStr">The string to trim.</param>
    /// <returns>Returns a mirror copy of the given string.</returns>
    std::string reverse(const std::string & iStr);

    /// <summary>
    /// Format a string.
    /// </summary>
    /// <param name="iFormat">The format of the string. Same as printf() format.</param>
    /// <returns>Returns a formatted string with the given parameters inserted.</returns>
    std::string format(const char * iFormat, ...);

  } //namespace strings
} //namespace ra

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const void * value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const std::string & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const char * value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const int8_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const uint8_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const int16_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const uint16_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const int32_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const uint32_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const int64_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const uint64_t & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const float & value);

/// <summary>
/// Streams a value to an existing string.
/// </summary>
/// <param name="str">The destination string.</param>
/// <param name="value">The value to append to the given string.</param>
/// <returns>Returns the given string.</returns>
std::string& operator<<(std::string& str, const double & value);

#endif //RA_STRING_H