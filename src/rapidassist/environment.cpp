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

#include "rapidassist/environment.h"
#include "rapidassist/strings.h"
#include <cstdlib> //for getenv()
#include <cstring> //for strlen()
#include <stdlib.h> //for setenv(), unsetenv()

namespace ra
{
  namespace environment
  {

    std::string getEnvironmentVariable(const char * iName)
    {
      if (iName == NULL)
        return std::string();
      const char * value = getenv(iName);
      if (value == NULL)
        return std::string();
      else
        return std::string(value);
    }

    bool setEnvironmentVariable(const char * iName, const char * iValue)
    {
      //validate invalid inputs
      if (iName == NULL || strlen(iName) == 0)
      {
        return false;
      }
 
#ifdef _WIN32
      std::string command;
      command.append(iName);
      command.append("=");
      if (iValue)
      {
        command.append(iValue);
      }
      int result = _putenv(command.c_str());
#elif __linux__
      int result = -1; //failure
      bool erase = (iValue == NULL || strlen(iValue) == 0);
      if (erase)
        result = unsetenv(iName);
      else
        result = setenv(iName, iValue, 1); //overwrite existing
#endif

      bool success = (result == 0);
      return success;
    }

    bool setEnvironmentVariable(const char * iName, const   int8_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const  uint8_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const  int16_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const uint16_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const  int32_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const uint32_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const  int64_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const uint64_t & iValue) { std::string tmp; tmp << iValue; return setEnvironmentVariable(iName, tmp.c_str() ); }
    bool setEnvironmentVariable(const char * iName, const    float & iValue)
    {
      const std::string & tmp = ra::strings::toStringLossy(iValue, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      return setEnvironmentVariable(iName, tmp.c_str() );
    }
    bool setEnvironmentVariable(const char * iName, const   double & iValue)
    {
      const std::string & tmp = ra::strings::toStringLossy(iValue, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON);
      return setEnvironmentVariable(iName, tmp.c_str() );
    }

    bool isProcess32Bit()
    {
#if defined(_WIN32) && !defined(_WIN64) //Windows
      return true;
#elif defined(__LP32__) || defined(_ILP32) //GCC
      return true;
#elif (__SIZEOF_POINTER__ == 4) //GCC only ?
      return true;
#elif ( __WORDSIZE == 32 ) //portable
      return true;
#else
      return false;
#endif
    }

    bool isProcess64Bit()
    {
#if defined(_WIN64) //Windows
      return true;
#elif defined(__LP64__) || defined(_LP64) //GCC
      return true;
#elif (__SIZEOF_POINTER__ == 8) //GCC only ?
      return true;
#elif ( __WORDSIZE == 64 ) //portable
      return true;
#else
      return false;
#endif
    }

    bool isConfigurationDebug()
    {
#ifdef NDEBUG
      return false;
#else
      return true;
#endif
    }

    bool isConfigurationRelease()
    {
#ifdef NDEBUG
      return true;
#else
      return false;
#endif
    }

    const char * getLineSeparator()
    {
#ifdef _WIN32
      return "\r\n";
#else
      return "\n";
#endif
    }

  } //namespace environment
} //namespace ra
