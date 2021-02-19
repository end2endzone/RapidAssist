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
#include <string.h> //for strlen()

namespace ra { namespace random {

  //Force initializing random number provider each time the application starts
  bool InitRandomProvider();
  static bool rand_initialized = InitRandomProvider();

  bool InitRandomProvider() {
    srand((unsigned int)time(NULL));
    return true;
  }

  int GetRandomInt() {
    return rand();
  }

  int GetRandomInt(int value_min, int value_max) {
    //limit the accepted value returned by rand() to allow uniform distribution of values
    //i.e:  if value_min=0 and value_max=7 and RAND_MAX=8 then 
    //      returned value 0 has probability twice as high as other numbers
    //      ( rand()==0 and rand()==8 both returns 0 )

    //compute maximum value of rand() to allow uniform distribution of values
    int max = RAND_MAX;
    int modulo = (value_max - value_min) + 1;
    if (max % modulo != (modulo - 1)) {
      //adjust max to get uniform distribution across value_min to value_max
      int diff = (max % modulo);
      max -= (diff + 1);
    }

    //get a random value within [0, max]
    int rand_value = rand();
    while (rand_value > max) {
      rand_value = rand();
    }

    int value = rand_value % modulo; //within [0, value_max-value_min]
    value += value_min;                  //within [value_min, value_max]

    return value;
  }

  double GetRandomDouble(double value_min, double value_max) {
    double f = (double)rand() / RAND_MAX;
    double value = value_min + f * (value_max - value_min);
    return value;
  }

  float GetRandomFloat(float value_min, float value_max) {
    float f = (float)rand() / RAND_MAX;
    float value = value_min + f * (value_max - value_min);
    return value;
  }

  std::string GetRandomString() {
    std::string rnd;
    GetRandomString(rnd, 20);
    return rnd;
  }

  void GetRandomString(std::string & value, size_t length) {
    static const char * defaultSymbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    GetRandomString(value, length, defaultSymbols);
  }

  std::string GetRandomString(size_t length) {
    std::string tmp;
    GetRandomString(tmp, length);
    return tmp;
  }

  void GetRandomString(std::string & value, size_t length, SymbolsFlags::Flags flags) {
    std::string symbols;

    if ((flags & SymbolsFlags::LETTERS_LOWERCASE) == SymbolsFlags::LETTERS_LOWERCASE)
      symbols.append("abcdefghijklmnopqrstuvwxyz");
    if ((flags & SymbolsFlags::LETTERS_UPPERCASE) == SymbolsFlags::LETTERS_UPPERCASE)
      symbols.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if ((flags & SymbolsFlags::NUMBERS) == SymbolsFlags::NUMBERS)
      symbols.append("0123456789");
    if ((flags & SymbolsFlags::SPECIAL_CHARACTERS) == SymbolsFlags::SPECIAL_CHARACTERS)
      symbols.append("!\"/$%?&*()_+-=\\:<>");

    if (symbols.size() == 0) {
      value = "";
      return;
    }

    GetRandomString(value, length, symbols.c_str());
  }

  std::string GetRandomString(size_t length, SymbolsFlags::Flags flags) {
    std::string tmp;
    GetRandomString(tmp, length, flags);
    return tmp;
  }

  void GetRandomString(std::string & value, size_t length, const char* symbols) {
    value.clear();
    if (symbols == NULL)
      return;

    size_t num_symbols = strlen(symbols);
    if (num_symbols == 0)
      return;

    value.reserve(length + 1);

    while (value.size() < length) {
      //generate a random character from symbols
      int index = GetRandomInt(0, ((int)num_symbols) - 1);
      const char & c = symbols[index];

      //add
      value.append(1, c);
    }
  }

  std::string GetRandomString(size_t length, const char* symbols) {
    std::string tmp;
    GetRandomString(tmp, length, symbols);
    return tmp;
  }

} //namespace random
} //namespace ra
