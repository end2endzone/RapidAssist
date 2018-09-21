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

namespace ra
{

  namespace strings
  {
    //template <class T>
    //inline bool parseValueT (const std::string& str, T & t)
    //{
    //  static const T ZERO = (T)0;
    //  static const T MULTIPLIER = (T)10;
    //  static const T SIGN_MULTIPLIER = (T)-1;
    //  bool parseOK = false;
    //  t = ZERO;
    //  for(size_t i=0; i<str.size(); i++)
    //  {
    //    char c = str[i];
    //    if (c >= '0' && c <= '9')
    //    {
    //      t *= MULTIPLIER;
    //      c -= '0'; //convert character to numeric value
    //      t += (T)c;
    //      parseOK = true;
    //    }
    //    else if (c == '-')
    //    {
    //      t *= SIGN_MULTIPLIER;
    //      parseOK = true;
    //    }
    //  }
    //  return parseOK;
    //}

    //Note: http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.2
    template <class T>
    inline std::string toStringT (const T & t)
    {
      std::stringstream out;
      out << t;
      std::string & s = out.str();
      return s;
    }

    template <class T>
    inline void parseT (const char * iValue, T & t)
    {
      std::string tmpString = iValue;
      std::istringstream inputStream(tmpString);
      inputStream >> t;
    }

    //specializations
    template<>
    inline std::string toStringT<unsigned char>(const unsigned char & t)
    {
      std::stringstream out;
      out << (int)t;
      std::string & s = out.str();
      return s;
    }
    template<>
    inline std::string toStringT<char>(const char & t)
    {
      std::stringstream out;
      out << (int)t;
      std::string & s = out.str();
      return s;
    }
    template<>
    inline std::string toStringT<int8_t>(const int8_t & t)
    {
      std::stringstream out;
      out << (int)t;
      std::string & s = out.str();
      return s;
    }
    template<>
    inline std::string toStringT<float>(const float & t)
    {
      std::stringstream out;
      out << std::setprecision(8) << t;
      std::string & s = out.str();
      return s;
    }
    template<>
    inline std::string toStringT<double>(const double & t)
    {
      std::stringstream out;
      out << std::setprecision(17) << t;
      std::string & s = out.str();
      return s;
    }

    template<>
    inline void parseT<unsigned char>(const char * iValue, unsigned char & t)
    {
      std::string tmpString = iValue;
      std::istringstream inputStream(tmpString);
      uint16_t tmp = 0;
      inputStream >> tmp;
      t = (unsigned char)tmp;
    }
    template<>
    inline void parseT<char>(const char * iValue, char & t)
    {
      std::string tmpString = iValue;
      std::istringstream inputStream(tmpString);
      int16_t tmp = 0;
      inputStream >> tmp;
      t = (char)tmp;
    }
    template<>
    inline void parseT<int8_t>(const char * iValue, int8_t & t)
    {
      std::string tmpString = iValue;
      std::istringstream inputStream(tmpString);
      int16_t tmp = 0;
      inputStream >> tmp;
      t = (char)tmp;
    }

    template <typename T>
    inline std::string toStringPrecision (const T & t, int numDigits)
    {
      //compute length (in digits) of integer part
      static const uint64_t DIGIT_SIZE = 10;
      int64_t integer_part = static_cast<int64_t>(t);
      int length = 0;
      while(integer_part > 0)
      {
        length++;
        integer_part /= DIGIT_SIZE;
      }

      int precision = numDigits - length;

      //build format for this type
      static const int FORMAT_SIZE = 8;
      char format[FORMAT_SIZE];
      format[0] = '%';
      format[1] = '.';
      if (precision >= 10)
      {
        format[2] = '0'+(char)(precision/10);
        format[3] = '0'+(char)(precision%10);
        format[4] = 'f';
        format[5] = '\0';
      }
      else
      {
        format[2] = '0'+(char)(precision%10);
        format[3] = 'f';
        format[4] = '\0';
      }

      //do the sprintf()
      static const int BUFFER_SIZE = 32;
      char tmp[BUFFER_SIZE];
      sprintf(tmp, format, t);
      std::string buffer = tmp;

      //remove non significant zeros
      buffer = ra::strings::trimRight(buffer, '0');
  
      //remove last character if it is a dot
      size_t last_char_offset = buffer.size()-1;
      if (!buffer.empty() && buffer[last_char_offset] == '.')
      {
        buffer.erase(last_char_offset, 1); //remove the dot
      }

      return buffer;
    }

    bool isNumeric(const char * iValue)
    {
      if (iValue == NULL)
        return false;

      bool foundDot = false;
      size_t length = strlen(iValue);
      for(size_t offset = 0; offset < length; offset++)
      {
        const char & c = iValue[offset];
        if (c >= '0' && c <= '9')
          continue; //valid
        if (c == '.' && !foundDot)
        {
          //only 1 dot character must be found in the string
          foundDot = true;
          continue; //valid
        }
        if ((c == '+' || c == '-'))
        {
          if (offset == 0)
          {
            //+ or - sign are accepted but must be the first character of the value
            continue; //valid
          }
        }

        return false; //invalid
      }
      return true;
    }

    int replace(std::string & iString, const char * iOldValue, const char * iNewValue)
    {
      std::string tmpOldValue = iOldValue;
      std::string tmpNewValue = iNewValue;

      int numOccurance = 0;

      if (tmpOldValue.size() > 0)
      {
        size_t startPos = 0;    
        size_t findPos = std::string::npos;
        do
        {
          findPos = iString.find(tmpOldValue, startPos);
          if (findPos != std::string::npos)
          {
            iString.replace(findPos, tmpOldValue.length(), tmpNewValue);
            startPos = findPos + tmpNewValue.length();
            numOccurance++;
          }
        }
        while (findPos != -1);
      }
      return numOccurance;
    }

    std::string toString(const   int8_t & value) { return toStringT(value); }
    std::string toString(const  uint8_t & value) { return toStringT(value); }
    std::string toString(const  int16_t & value) { return toStringT(value); }
    std::string toString(const uint16_t & value) { return toStringT(value); }
    std::string toString(const  int32_t & value) { return toStringT(value); }
    std::string toString(const uint32_t & value) { return toStringT(value); }
    std::string toString(const  int64_t & value) { return toStringT(value); }
    std::string toString(const uint64_t & value) { return toStringT(value); }
    std::string toString(const    float & value) { return toStringT(value); }
    std::string toString(const   double & value) { return toStringT(value); }

    bool parseValue(const std::string& str, uint64_t & oValue)
    {
      //return parseValueT(str, oValue);
      parseT(str.c_str(), oValue);

      //verify
      std::string & tmp = toString(oValue);
      bool success = (tmp == str);
      return success;
    }

    bool parse(const std::string& str,   int8_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,  uint8_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,  int16_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str, uint16_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,  int32_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str, uint32_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,  int64_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str, uint64_t & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,    float & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }
    bool parse(const std::string& str,   double & oValue) { parseT(str.c_str(), oValue); /*verify*/ std::string & tmp = toString(oValue); bool success = (tmp == str); return success; }

    std::string capitalizeFirstCharacter(const std::string & iValue)
    {
      std::string copy = iValue;
      if (!copy.empty())
      {
        copy[0] = (char)toupper(copy[0]);
      }
      return copy;
    }

    std::string uppercase(const std::string & iValue)
    {
      std::string copy = iValue;
      for(size_t i=0; i<copy.size(); i++)
      {
        copy[i] = (char)toupper(copy[i]);
      }
      return copy;
    }

    std::string lowercase(const std::string & iValue)
    {
      std::string copy = iValue;
      for(size_t i=0; i<copy.size(); i++)
      {
        copy[i] = (char)tolower(copy[i]);
      }
      return copy;
    }

    void removeEOL(char * iBuffer)
    {
      if (iBuffer == NULL)
        return;

      int index = 0;
      while(iBuffer[index] != '\0')
      {
        if (iBuffer[index] == '\n' && iBuffer[index+1] == '\r' && iBuffer[index+2] == '\0') //Windows
          iBuffer[index] = '\0';
        else if (iBuffer[index] == '\r' && iBuffer[index+1] == '\n' && iBuffer[index+2] == '\0') //Windows
          iBuffer[index] = '\0';
        else if (iBuffer[index] == '\n' && iBuffer[index+1] == '\0') // UNIX
          iBuffer[index] = '\0';
        else if (iBuffer[index] == '\r' && iBuffer[index+1] == '\0') // OLD MAC
          iBuffer[index] = '\0';

        index++;
      }
    }

    StringVector splitString(const std::string & iText, char iSplitCharacter)
    {
      char pattern[] = {iSplitCharacter, '\0'};
      StringVector list;
      splitString(list, iText, pattern);
      return list;
    }

    StringVector splitString(const std::string & iText, const char * iSplitPattern)
    {
      StringVector list;
      splitString(list, iText, iSplitPattern);
      return list;
    }

    void splitString(StringVector & oList, const std::string & iText, char iSplitCharacter)
    {
      char pattern[] = {iSplitCharacter, '\0'};
      splitString(oList, iText, pattern);
    }

    void splitString(StringVector & oList, const std::string & iText, const char * iSplitPattern)
    {
      oList.clear();

      //validate invalue split pattern
      if (iSplitPattern == NULL || strlen(iSplitPattern) == 0)
      {
        oList.push_back(iText);
        return;
      }

      std::string accumulator;
      std::string pattern = iSplitPattern;
      for(size_t i=0; i<iText.size(); i++)
      {
        const char * substring = &iText[i];
        if (strncmp(substring, pattern.c_str(), pattern.size()) == 0)
        {
          //found a split pattern

          //flush current accumulator
          if (accumulator != "")
          {
            oList.push_back(accumulator);
            accumulator = "";
          }

          //does iTest starts with a separator?
          if (i == 0)
          {
            oList.push_back("");
          }

          //does this separator follows another separator?
          if (i >= pattern.size() && strncmp(&iText[i - pattern.size()], pattern.c_str(), pattern.size()) == 0)
          {
            //consecutive separators
            oList.push_back("");
          }

          i += pattern.size();

          //does iText ends with a separator?
          if (iText[i] == '\0')
          {
            oList.push_back("");
          }

          i--; //since the next loop will increase i by 1
        }
        else
        {
          char tmp[] = { iText[i], '\0' };
          accumulator.append(tmp);
        }
      }

      //flush current accumulator
      if (accumulator != "")
      {
        oList.push_back(accumulator);
        accumulator = "";
      }
    }

    std::string joinString(const StringVector & iList, const char * iSeparator)
    {
      std::string output;
      for(size_t i=0; i<iList.size(); i++)
      {
        const std::string & value = iList[i];
        if (i > 0 && iSeparator != NULL)
          output.append(iSeparator);
        output.append(value);
      }
      return output;
    }

    std::string trim(const std::string & iStr)
    {
      return trimLeft(trimRight(iStr));
    }

    std::string trim(const std::string & iStr, const char iChar)
    {
      return trimLeft(trimRight(iStr, iChar), iChar);
    }

    std::string trimRight(const std::string & iStr)
    {
      return trimRight(iStr, ' ');
    }

    std::string trimLeft(const std::string & iStr)
    {
      return trimLeft(iStr, ' ');
    }

    std::string trimRight(const std::string & iStr, const char iChar)
    {
      static const size_t SIZE_T_MAX = std::numeric_limits< size_t >::max();
      
      std::string tmp = iStr;

      if (iChar == '\0')
        return tmp;

      size_t size = iStr.size();
      if (size)
      {
        size_t loopStart = size - 1;
        size_t loopEnd = 0;
        for(size_t i = loopStart; i >= loopEnd && i != SIZE_T_MAX; i--)
        {
          if (tmp[i] == iChar)
          {
            tmp.erase(i, 1);
          }
          else
          {
            //no need to continue looking
            break;
          }
        }
      }
      return tmp;
    }

    std::string trimLeft(const std::string & iStr, const char iChar)
    {
      std::string tmp = iStr;

      if (iChar == '\0')
        return tmp;

      size_t size = iStr.size();
      if (size)
      {
        while(!tmp.empty() && tmp[0] == iChar)
        {
          tmp.erase(0, 1);
        }
      }
      return tmp;
    }

    std::string reverse(const std::string & iStr)
    {
      std::string tmp = iStr;

      size_t size = iStr.size();
      if (size)
      {
        size_t loopStart = 0;
        size_t loopEnd = (size - 1) / 2;
        for(size_t i = loopStart; i <= loopEnd; i++)
        {
          if (i != (size - i - 1))
          {
            ra::generics::swap(tmp[i], tmp[size - i - 1]);
          }
        }
      }
      return tmp;
    }

    std::string format(const char * iFormat, ...)
    {
      std::string output;

      va_list args;
      va_start(args, iFormat);

      static const int BUFFER_SIZE = 10240;
      char buffer[BUFFER_SIZE];
      buffer[0] = '\0';
      vsnprintf(buffer, BUFFER_SIZE, iFormat, args);
      output = buffer;

      va_end (args);

      return output;
    }

  } //namespace strings
} //namespace ra

std::string& operator<<(std::string& str, const void * value)
{
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

std::string& operator<<(std::string& str, const std::string & value)
{
  str.append(value);
  return str;
}

std::string& operator<<(std::string& str, const char * value)
{
  str.append(value);
  return str;
}

std::string& operator<<(std::string& str, const int16_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const uint16_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const int8_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const uint8_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const int32_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const uint32_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const int64_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const uint64_t & value)
{
  //std::stringstream out;
  //out << value;
  //str.append( out.str() );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const float & value)
{
  //std::string & buffer = ra::strings::toStringPrecision(value, 8);
  //str.append( buffer );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}

std::string& operator<<(std::string& str, const double & value)
{
  //std::string & buffer = ra::strings::toStringPrecision(value, 17);
  //str.append( buffer );
  std::string & out = ra::strings::toStringT(value);
  str.append( out );
  return str;
}
