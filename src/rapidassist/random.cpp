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

#include "rapidassist/random.h"

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
      //limit the accepted value returned by rand() to allow uniform distribution of values
      //i.e:  if iMin=0 and iMax=7 and RAND_MAX=8 then 
      //      returned value 0 has probability twice as high as other numbers
      //      ( rand()==0 and rand()==8 both returns 0 )

      //compute maximum value of rand() to allow uniform distribution of values
      int max = RAND_MAX;
      int modulo = (iMax - iMin) + 1;
      if (max % modulo != (modulo-1))
      {
        //adjust max to get uniform distribution across iMin to iMax
        int diff = (max % modulo);
        max -= (diff+1);
      }

      //get a random value within [0, max]
      int randValue = rand();
      while(randValue > max)
      {
        randValue = rand();
      }

      int value = randValue % modulo; //within [0, iMax-iMin]
      value += iMin;                  //within [iMin, iMax]

      return value;
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

    void getRandomString(std::string & oValue, size_t iMaxLen)
    {
      static const char * defaultSymbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      getRandomString(oValue, iMaxLen, defaultSymbols);
    }

    std::string getRandomString(size_t iMaxLen)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen);
      return tmp;
    }

    void getRandomString(std::string & oValue, size_t iMaxLen, SymbolsFlags::Flags iFlags)
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

    std::string getRandomString(size_t iMaxLen, SymbolsFlags::Flags iFlags)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen, iFlags);
      return tmp;
    }

    void getRandomString(std::string & oValue, size_t iMaxLen, const char* iSymbols)
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

    std::string getRandomString(size_t iMaxLen, const char* iSymbols)
    {
      std::string tmp;
      getRandomString(tmp, iMaxLen, iSymbols);
      return tmp;
    }


  } //namespace random
} //namespace ra
