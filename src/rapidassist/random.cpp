#include "random.h"
#include <cstdlib>  //for random
#include <ctime>    //for random

namespace ra
{
  namespace random
  {
    //Force initializing random number provider each time the application starts
    bool initRandomProvider();
    static bool randInitialized = initRandomProvider();

    bool initRandomProvider()
    {
      srand((unsigned int)time(NULL));
      return true;
    }

    int getRandomInt()
    {
      return rand();
    }

    int getRandomInt(int iMin, int iMax)
    {
      int range = (iMax - iMin)+1;
      return iMin+int(range * rand() / (RAND_MAX + 1.0)); 
    }

    double getRandomDouble(double iMin, double iMax)
    {
      double f = (double)rand() / RAND_MAX;
      double value = iMin + f * (iMax - iMin);
      return value;
    }

    float getRandomFloat(float iMin, float iMax)
    {
      float f = (float)rand() / RAND_MAX;
      float value = iMin + f * (iMax - iMin);
      return value;
    }

    std::string getRandomString()
    {
      std::string rnd;
      getRandomString(rnd, 20);
      return rnd;
    }

    void getRandomString(std::string & oValue, int iMaxLen)
    {
      static const char * defaultSymbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      getRandomString(oValue, iMaxLen, defaultSymbols);
    }

    std::string getRandomString(int iMaxLen)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen);
      return tmp;
    }

    void getRandomString(std::string & oValue, int iMaxLen, SymbolsFlags::Flags iFlags)
    {
      std::string symbols;

      if ((iFlags & SymbolsFlags::Letters) == SymbolsFlags::Letters)
        symbols.append("abcdefghijklmnopqrstuvwxyz");
      if ((iFlags & SymbolsFlags::LETTERS) == SymbolsFlags::LETTERS)
        symbols.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      if ((iFlags & SymbolsFlags::Numbers) == SymbolsFlags::Numbers)
        symbols.append("0123456789");
      if ((iFlags & SymbolsFlags::SpecialCharacters) == SymbolsFlags::SpecialCharacters)
        symbols.append("!\"/$%?&*()_+-=\\:<>");

      if (symbols.size() == 0)
      {
        oValue = "";
        return;
      }

      getRandomString(oValue, iMaxLen, symbols.c_str());
    }

    std::string getRandomString(int iMaxLen, SymbolsFlags::Flags iFlags)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen, iFlags);
      return tmp;
    }

    void getRandomString(std::string & oValue, int iMaxLen, const char* iSymbols)
    {
      std::string symbols = iSymbols;
      int numSymbols = (int)symbols.size();

      oValue.reserve(iMaxLen+1);

      while (oValue.size() < (size_t)iMaxLen)
      {
        //generate a random character from iSymbols
        int index = getRandomInt(0, numSymbols-1);
        char tmpStr[] = { iSymbols[index], 0 };

        //add
        oValue.append(tmpStr);
      }
    }

    std::string getRandomString(int iMaxLen, const char* iSymbols)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen, iSymbols);
      return tmp;
    }


  } //namespace random
} //namespace ra
