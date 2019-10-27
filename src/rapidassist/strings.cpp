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

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/generics.h"

#include <sstream>  //for std::stringstream
#include <string.h> //for strlen()
#include <limits>   //for std::numeric_limits
#include <stdarg.h> //for ...
#include <stdio.h>  //for vsnprintf()
#include <iomanip>  //for std::setprecision()
#include <cmath>    //for abs()

namespace ra { namespace strings {

  //constants
  const int  FLOAT_TOSTRING_LOSSLESS_PRECISION = 9;
  const int DOUBLE_TOSTRING_LOSSLESS_PRECISION = 17;
  const  float  FLOAT_TOSTRING_LOSSY_EPSILON = 0.0000001f;
  const double DOUBLE_TOSTRING_LOSSY_EPSILON = 0.0000000000000001;

  //Note: http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.2
  template <class T>
  inline std::string toStringT(const T & t) {
    std::stringstream out;
    out << t;
    const std::string & s = out.str();
    return s;
  }

  template <class T>
  inline void parseT(const std::string & iValue, T & t) {
    std::istringstream input_stream(iValue);
    input_stream >> t;
  }

  //specializations
  template<>
  inline std::string toStringT<unsigned char>(const unsigned char & t) {
    std::stringstream out;
    out << (int)t;
    const std::string & s = out.str();
    return s;
  }
  template<>
  inline std::string toStringT<char>(const char & t) {
    std::stringstream out;
    out << (int)t;
    const std::string & s = out.str();
    return s;
  }
  template<>
  inline std::string toStringT<int8_t>(const int8_t & t) {
    std::stringstream out;
    out << (int)t;
    const std::string & s = out.str();
    return s;
  }
  template<>
  inline std::string toStringT<float>(const float & t) {
    //To get a lossless conversion from float to string, a precision of at least 8 is required.
    //However, in order to get the maximum number of digits while printing the number (((float)14263 / 32767) + 1000000.0f), which is displayed as 1000000.4 in Visual Studio 2010, a precision of 11 is required.
    //If we toString() using different precision, we get the following:
    //   6 := 1e+006
    //   7 := 1000000
    //   8 := 1000000.4
    //   9 := 1000000.44
    //  10 := 1000000.438
    //  11 := 1000000.4375
    //  12 := 1000000.4375
    //  13 := ...
    //  14 := ...
    //  15 := ...
    //  16 := ...
    //  17 := ...
    //  
    //The required precision to get a lossless toString() is different for all values.
    //For instance, the value "(((float)1234 / 9999) + 1000.0f)" which in theory is 1000.1234 1234 1234 1234 1234... requires a precision of 17:
    //   6 := 1000.12
    //   7 := 1000.123
    //   8 := 1000.1234
    //   9 := 1000.12341
    //  10 := 1000.123413
    //  11 := 1000.1234131
    //  12 := 1000.12341309
    //  13 := 1000.123413086
    //  14 := 1000.1234130859
    //  15 := 1000.12341308594
    //  16 := 1000.123413085938
    //  17 := 1000.1234130859375
    //  18 := 1000.1234130859375
    //  19 := ...
    //  20 := ...
    //  21 := ...
    //  
    //The value (9998877665544332211.0f / 1000000000.0f) requires a precision of 10:
    //   6 := 9.99888e+009
    //   7 := 9.998878e+009
    //   8 := 9.9988777e+009
    //   9 := 9.9988777e+009
    //  10 := 9998877696
    //  11 := 9998877696
    //  12 := ...
    //  
    //The value (998877654321.0f / 1000000000.0f) requires a precision of 17:
    //   6 := 998.878
    //   7 := 998.8776
    //   8 := 998.87762
    //   9 := 998.877625
    //  10 := 998.8776245
    //  11 := 998.87762451
    //  12 := 998.877624512
    //  13 := 998.8776245117
    //  14 := 998.87762451172
    //  15 := 998.877624511719
    //  16 := 998.8776245117188
    //  17 := 998.87762451171875
    //  18 := 998.87762451171875
    //  19 := ...
    //
    //However, if we simply set 5.3f (which is in fact 5.3000002) and we use a too much precision, we get only rounding errors:
    //   6 := 5.3
    //   7 := 5.3
    //   8 := 5.3000002
    //   9 := 5.30000019
    //  10 := 5.300000191
    //  11 := 5.3000001907
    //  12 := 5.30000019073
    //  13 := 5.300000190735
    //  14 := 5.3000001907349
    //  15 := 5.30000019073486
    //  16 := 5.300000190734863
    //  17 := 5.3000001907348633
    //  18 := 5.3000001907348633
    //  19 := ...
    //  
    //
    //In the end, we can never know what is the required precision. Most use of floating point should probably target a practical range of -1 million to +1 million.
    //Adjusting precision so that a random number between -1 million and +1 million can be converted to string and back to float without loosing quality.
    //A precision of 9 is required for this.

    std::stringstream out;
    out << std::setprecision(FLOAT_TOSTRING_LOSSLESS_PRECISION) << t;
    const std::string & s = out.str();
    return s;
  }
  template<>
  inline std::string toStringT<double>(const double & t) {
    //note lossless conversion
    std::stringstream out;
    out << std::setprecision(DOUBLE_TOSTRING_LOSSLESS_PRECISION) << t;
    const std::string & s = out.str();
    return s;
  }

  template<>
  inline void parseT<unsigned char>(const std::string & iValue, unsigned char & t) {
    std::istringstream input_stream(iValue);
    uint16_t tmp = 0;
    input_stream >> tmp;
    t = (unsigned char)tmp;
  }
  template<>
  inline void parseT<char>(const std::string & iValue, char & t) {
    std::istringstream input_stream(iValue);
    int16_t tmp = 0;
    input_stream >> tmp;
    t = (char)tmp;
  }
  template<>
  inline void parseT<int8_t>(const std::string & iValue, int8_t & t) {
    std::istringstream input_stream(iValue);
    int16_t tmp = 0;
    input_stream >> tmp;
    t = (char)tmp;
  }

  template <typename T>
  inline std::string toStringDigits (const T & t, int num_digits) {
    if (num_digits < 0)
      num_digits = 0;
    if (num_digits > 99)
      num_digits = 99;

    //build format for this type
    static const int FORMAT_SIZE = 8;
    char format[FORMAT_SIZE];
    format[0] = '%';
    format[1] = '.';
    if (num_digits >= 10) {
      format[2] = '0'+(char)(num_digits/10);
      format[3] = '0'+(char)(num_digits%10);
      format[4] = 'f';
      format[5] = '\0';
    }
    else {
      format[2] = '0'+(char)(num_digits%10);
      format[3] = 'f';
      format[4] = '\0';
    }

    //do the sprintf()
    static const int BUFFER_SIZE = 32;
    char tmp[BUFFER_SIZE];
    sprintf(tmp, format, t);
    std::string buffer = tmp;

    return buffer;
  }

  bool isNumeric(const char * iValue) {
    if (iValue == NULL)
      return false;

    bool found_dot = false;
    size_t length = strlen(iValue);
    for (size_t offset = 0; offset < length; offset++) {
      const char & c = iValue[offset];
      if (c >= '0' && c <= '9')
        continue; //valid
      if (c == '.' && !found_dot) {
        //only 1 dot character must be found in the string
        found_dot = true;
        continue; //valid
      }
      if ((c == '+' || c == '-')) {
        if (offset == 0) {
          //+ or - sign are accepted but must be the first character of the value
          continue; //valid
        }
      }

      return false; //invalid
    }
    return true;
  }

  int replace(std::string & iString, const std::string & iOldValue, const std::string & iNewValue) {
    int num_occurance = 0;

    if (iOldValue.size() > 0) {
      size_t start_pos = 0;
      size_t find_pos = std::string::npos;
      do {
        find_pos = iString.find(iOldValue, start_pos);
        if (find_pos != std::string::npos) {
          iString.replace(find_pos, iOldValue.length(), iNewValue);
          start_pos = find_pos + iNewValue.length();
          num_occurance++;
        }
      } while (find_pos != -1);
    }
    return num_occurance;
  }

  std::string toString(const bool & value) {
    if (value)
      return std::string("true");
    return std::string("false");
  }

  //default base type excepted floating points
  std::string toString(const   int8_t & value) { return toStringT(value); }
  std::string toString(const  uint8_t & value) { return toStringT(value); }
  std::string toString(const  int16_t & value) { return toStringT(value); }
  std::string toString(const uint16_t & value) { return toStringT(value); }
  std::string toString(const  int32_t & value) { return toStringT(value); }
  std::string toString(const uint32_t & value) { return toStringT(value); }
  std::string toString(const  int64_t & value) { return toStringT(value); }
  std::string toString(const uint64_t & value) { return toStringT(value); }

  //floating point, lossless conversion
  std::string toStringLossless(const    float & value) { return toStringT(value); }
  std::string toStringLossless(const   double & value) { return toStringT(value); }

  //floating point, lossy conversion
  std::string toStringLossy(const    float & value, const  float & epsilon) {
    for (int digits = 0; digits < FLOAT_TOSTRING_LOSSLESS_PRECISION; digits++) {
      const std::string & str = toStringFormatted(value, digits);
      float parsed_value = 0.0f;
      parseT(str, parsed_value); //do not look at the parsing result since we are aiming at a lossy conversion.
      float diff = std::abs(parsed_value - value);
      if (diff <= epsilon) {
        //this is the shortest representation
        return str;
      }
    }

    const std::string & str = toStringLossless(value); //lossless conversion
    return str;
  }

  std::string toStringLossy(const   double & value, const double & epsilon) {
    for (int digits = 0; digits < DOUBLE_TOSTRING_LOSSLESS_PRECISION; digits++) {
      const std::string & str = toStringFormatted(value, digits);
      double parsed_value = 0.0;
      parseT(str, parsed_value); //do not look at the parsing result since we are aiming at a lossy conversion.
      double diff = std::abs(parsed_value - value);
      if (diff <= epsilon) {
        //this is the shortest representation
        return str;
      }
    }

    const std::string & str = toStringLossless(value); //lossless conversion
    return str;
  }

  //floating point, formatted output
  std::string toStringFormatted(const    float & value, int digits) {
    const std::string & tmp = toStringDigits(value, digits);
    return tmp;
  }
  std::string toStringFormatted(const   double & value, int digits) {
    const std::string & tmp = toStringDigits(value, digits);
    return tmp;
  }

  //floating point, default toString() implementation
  std::string toString(const    float & value) { return toStringLossy(value, FLOAT_TOSTRING_LOSSY_EPSILON); }
  std::string toString(const   double & value) { return toStringLossy(value, DOUBLE_TOSTRING_LOSSY_EPSILON); }

  bool parseBoolean(const std::string & str) {
    if (str == "1")
      return true;
    else if (ra::strings::uppercase(str) == "TRUE")
      return true;
    else if (ra::strings::uppercase(str) == "YES")
      return true;
    else if (ra::strings::uppercase(str) == "Y")
      return true;
    else if (ra::strings::uppercase(str) == "ON")
      return true;
    return false;
  }

  bool parse(const std::string& str, int8_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, uint8_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, int16_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, uint16_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, int32_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, uint32_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, int64_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, uint64_t & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toString(oValue);         bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, float & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toStringLossless(oValue); bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, double & oValue) { parseT(str, oValue); /*verify*/ const std::string & tmp = toStringLossless(oValue); bool lossless = (tmp == str); return lossless; }
  bool parse(const std::string& str, bool & oValue) {
    //first try to parse the value as a string
    std::string upper_str = ra::strings::uppercase(str);
    if (upper_str == "TRUE") { oValue = true; return true; }
    if (upper_str == "YES") { oValue = true; return true; }
    if (upper_str == "Y") { oValue = true; return true; }
    if (upper_str == "ON") { oValue = true; return true; }
    if (upper_str == "FALSE") { oValue = false; return true; }
    if (upper_str == "NO") { oValue = false; return true; }
    if (upper_str == "N") { oValue = false; return true; }
    if (upper_str == "OFF") { oValue = false; return true; }

    //then try to parse the value as a signed integer
    int64_t signed_integer = 0;
    bool parsed = parse(str, signed_integer);
    if (parsed) {
      oValue = (signed_integer != 0);
      return true;
    }

    //try to parse the value as an unsigned integer
    uint64_t unsigned_integer = 0;
    parsed = parse(str, unsigned_integer);
    if (parsed) {
      oValue = (unsigned_integer != 0);
      return true;
    }

    //failed parsing
    return false;
  }

  std::string capitalizeFirstCharacter(const std::string & iValue) {
    std::string copy = iValue;
    if (!copy.empty()) {
      copy[0] = (char)toupper(copy[0]);
    }
    return copy;
  }

  std::string uppercase(const std::string & iValue) {
    std::string copy = iValue;
    for (size_t i = 0; i < copy.size(); i++) {
      copy[i] = (char)toupper(copy[i]);
    }
    return copy;
  }

  std::string lowercase(const std::string & iValue) {
    std::string copy = iValue;
    for (size_t i = 0; i < copy.size(); i++) {
      copy[i] = (char)tolower(copy[i]);
    }
    return copy;
  }

  void removeEOL(char * iBuffer) {
    if (iBuffer == NULL)
      return;

    int index = 0;
    while (iBuffer[index] != '\0') {
      if (iBuffer[index] == '\n' && iBuffer[index + 1] == '\r' && iBuffer[index + 2] == '\0') //Windows
        iBuffer[index] = '\0';
      else if (iBuffer[index] == '\r' && iBuffer[index + 1] == '\n' && iBuffer[index + 2] == '\0') //Windows
        iBuffer[index] = '\0';
      else if (iBuffer[index] == '\n' && iBuffer[index + 1] == '\0') // UNIX
        iBuffer[index] = '\0';
      else if (iBuffer[index] == '\r' && iBuffer[index + 1] == '\0') // OLD MAC
        iBuffer[index] = '\0';

      index++;
    }
  }

  StringVector split(const std::string & iText, char iSplitCharacter) {
    char pattern[] = { iSplitCharacter, '\0' };
    StringVector list;
    split(list, iText, pattern);
    return list;
  }

  StringVector split(const std::string & iText, const char * iSplitPattern) {
    StringVector list;
    split(list, iText, iSplitPattern);
    return list;
  }

  void split(StringVector & oList, const std::string & iText, char iSplitCharacter) {
    char pattern[] = { iSplitCharacter, '\0' };
    split(oList, iText, pattern);
  }

  void split(StringVector & oList, const std::string & iText, const char * iSplitPattern) {
    oList.clear();

    //validate invalue split pattern
    if (iSplitPattern == NULL || strlen(iSplitPattern) == 0) {
      oList.push_back(iText);
      return;
    }

    std::string accumulator;
    std::string pattern = iSplitPattern;
    for (size_t i = 0; i < iText.size(); i++) {
      const char * substring = &iText[i];
      if (strncmp(substring, pattern.c_str(), pattern.size()) == 0) {
        //found a split pattern

        //flush current accumulator
        if (accumulator != "") {
          oList.push_back(accumulator);
          accumulator = "";
        }

        //does iTest starts with a separator?
        if (i == 0) {
          oList.push_back("");
        }

        //does this separator follows another separator?
        if (i >= pattern.size() && strncmp(&iText[i - pattern.size()], pattern.c_str(), pattern.size()) == 0) {
          //consecutive separators
          oList.push_back("");
        }

        i += pattern.size();

        //does iText ends with a separator?
        if (iText[i] == '\0') {
          oList.push_back("");
        }

        i--; //since the next loop will increase i by 1
      }
      else {
        const char & c = iText[i];
        accumulator.append(1, c);
      }
    }

    //flush current accumulator
    if (accumulator != "") {
      oList.push_back(accumulator);
      accumulator = "";
    }
  }

  std::string join(const StringVector & iList, const char * iSeparator) {
    std::string output;
    for (size_t i = 0; i < iList.size(); i++) {
      const std::string & value = iList[i];
      if (i > 0 && iSeparator != NULL)
        output.append(iSeparator);
      output.append(value);
    }
    return output;
  }

  std::string trim(const std::string & iStr) {
    return trimLeft(trimRight(iStr));
  }

  std::string trim(const std::string & iStr, const char iChar) {
    return trimLeft(trimRight(iStr, iChar), iChar);
  }

  std::string trimRight(const std::string & iStr) {
    return trimRight(iStr, ' ');
  }

  std::string trimLeft(const std::string & iStr) {
    return trimLeft(iStr, ' ');
  }

  std::string trimRight(const std::string & iStr, const char iChar) {
    static const size_t SIZE_T_MAX = std::numeric_limits< size_t >::max();

    std::string tmp = iStr;

    if (iChar == '\0')
      return tmp;

    size_t size = iStr.size();
    if (size) {
      size_t loop_start = size - 1;
      size_t loop_end = 0;
      for(size_t i = loop_start; i >= loop_end && i != SIZE_T_MAX; i--) {
        if (tmp[i] == iChar) {
          tmp.erase(i, 1);
        }
        else {
          //no need to continue looking
          break;
        }
      }
    }
    return tmp;
  }

  std::string trimLeft(const std::string & iStr, const char iChar) {
    std::string tmp = iStr;

    if (iChar == '\0')
      return tmp;

    size_t size = iStr.size();
    if (size) {
      while (!tmp.empty() && tmp[0] == iChar) {
        tmp.erase(0, 1);
      }
    }
    return tmp;
  }

  std::string reverse(const std::string & iStr) {
    std::string tmp = iStr;

    size_t size = iStr.size();
    if (size) {
      size_t loop_start = 0;
      size_t loop_end = (size - 1) / 2;
      for(size_t i = loop_start; i <= loop_end; i++) {
        if (i != (size - i - 1)) {
          ra::generics::swap(tmp[i], tmp[size - i - 1]);
        }
      }
    }
    return tmp;
  }

  std::string format(const char * iFormat, ...) {
    std::string output;

    va_list args;
    va_start(args, iFormat);

    static const int BUFFER_SIZE = 10240;
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';
    vsnprintf(buffer, BUFFER_SIZE, iFormat, args);
    output = buffer;

    va_end(args);

    return output;
  }

} //namespace strings
} //namespace ra

std::string& operator<<(std::string& str, const void * value) {
  size_t address = reinterpret_cast<size_t>(value);
  char buffer[1024];
#ifdef _WIN32
  if (ra::environment::isProcess32Bit())
    sprintf(buffer, "0x%08IX", address);
  else if (ra::environment::isProcess64Bit())
    sprintf(buffer, "0x%016IX", address);
#else
  if (ra::environment::isProcess32Bit())
    sprintf(buffer, "0x%08zX", address);
  else if (ra::environment::isProcess64Bit())
    sprintf(buffer, "0x%016zX", address);
#endif
  str.append(buffer);
  return str;
}

std::string& operator<<(std::string& str, const std::string & value) {
  str.append(value);
  return str;
}

std::string& operator<<(std::string& str, const char * value) {
  str.append(value);
  return str;
}

std::string& operator<<(std::string& str, const int16_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const uint16_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const int8_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const uint8_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const int32_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const uint32_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const int64_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const uint64_t & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const float & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}

std::string& operator<<(std::string& str, const double & value) {
  const std::string & out = ra::strings::toString(value);
  str.append(out);
  return str;
}
