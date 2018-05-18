#include "rapidassist/environment.h"
#include <cstdlib> //for getenv()

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
