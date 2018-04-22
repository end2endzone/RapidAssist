#include "rastring.h"

#include <sstream>  //for std::stringstream
#include <string.h> //for strlen()

namespace ra
{

  namespace strings
  {
    template <class T>
    inline bool parseValueT (const std::string& str, T & t)
    {
      static const T ZERO = (T)0;
      static const T MULTIPLIER = (T)10;
      static const T SIGN_MULTIPLIER = (T)-1;
      bool parseOK = false;
      t = ZERO;
      for(size_t i=0; i<str.size(); i++)
      {
        char c = str[i];
        if (c >= '0' && c <= '9')
        {
          t *= MULTIPLIER;
          c -= '0'; //convert character to numeric value
          t += (T)c;
          parseOK = true;
        }
        else if (c == '-')
        {
          t *= SIGN_MULTIPLIER;
          parseOK = true;
        }
      }
      return parseOK;
    }

    //Note: http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.2
    template <class T>
    inline std::string toStringT (const T & t)
    {
      std::stringstream out;
      out << t;
      std::string s;
      s = out.str().c_str();
      return s;
    }

    //template <class T>
    //inline void toT (const char * iValue, T & t)
    //{
    //  std::string tmpString = iValue;
    //  std::istringstream inputStream(tmpString);
    //  inputStream >> t;
    //}

    //specializations
    template<>
    inline std::string toStringT<unsigned char>(const unsigned char & t)
    {
      std::stringstream out;
      out << (int)t;
      std::string s;
      s = out.str();
      return s;
    }
    template<>
    inline std::string toStringT<char>(const char & t)
    {
      std::stringstream out;
      out << (int)t;
      std::string s;
      s = out.str();
      return s;
    }

    //template<>
    //inline void toT<unsigned char>(const char * iValue, unsigned char & t)
    //{
    //  std::string tmpString = iValue;
    //  std::istringstream inputStream(tmpString);
    //  uint16 tmp = 0;
    //  inputStream >> tmp;
    //  t = (unsigned char)tmp;
    //}
    //template<>
    //inline void toT<char>(const char * iValue, char & t)
    //{
    //  std::string tmpString = iValue;
    //  std::istringstream inputStream(tmpString);
    //  sint16 tmp = 0;
    //  inputStream >> tmp;
    //  t = (char)tmp;
    //}

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

    int strReplace(std::string & iString, const char * iOldValue, const char * iNewValue)
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

    std::string toString(const uint64_t & value)
    {
      return toStringT(value);
    }

    bool parseValue(const std::string& str, uint64_t & oValue)
    {
      return parseValueT(str, oValue);
    }

    std::string capitalizeFirstCharacter(const std::string & iValue)
    {
      std::string copy = iValue;
      if (!copy.empty())
      {
        copy[0] = (char)toupper(copy[0]);
      }
      return copy;
    }

    std::string uppercase(const std::string & value)
    {
      std::string copy = value;
      for(size_t i=0; i<copy.size(); i++)
      {
        copy[i] = (char)toupper(copy[i]);
      }
      return copy;
    }

    std::string lowercase(const std::string & value)
    {
      std::string copy = value;
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
        if (iBuffer[index] == 13 && iBuffer[index+1] == 10)
          iBuffer[index] = '\0';
        else if (iBuffer[index] == 10 && iBuffer[index+1] == 13)
          iBuffer[index] = '\0';
        else if (iBuffer[index] == 10 && iBuffer[index+1] == '\0')
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

  } //namespace strings
} //namespace ra

std::string& operator<<(std::string& str, const void * value)
{
  char buffer[1024];
  if (sizeof(void*) == 4)
    sprintf(buffer, "0x%08X", value);
  else if (sizeof(void*) == 8)
    sprintf(buffer, "0x%016X", value);
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
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const uint16_t & value)
{
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const int8_t & value)
{
  std::stringstream out;
  out << (int16_t)value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const uint8_t & value)
{
  std::stringstream out;
  out << (uint16_t)value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const int32_t & value)
{
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const uint32_t & value)
{
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const int64_t & value)
{
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}

std::string& operator<<(std::string& str, const uint64_t & value)
{
  std::stringstream out;
  out << value;
  str.append( out.str() );
  return str;
}
