#ifndef RA_RANDOM_H
#define RA_RANDOM_H

#include <string>

namespace ra
{
  namespace random
  {

    ///<summary>
    ///Returns a random number between 0 (inclusive) and RAND_MAX (inclusive).
    ///</summary>
    ///<return>Returns a random number between 0 and RAND_MAX.<return>
    int getRandomInt();

    ///<summary>
    ///Returns a random number between iMin (inclusive) and iMax (inclusive).
    ///</summary>
    ///<param name="iMin">The minimum value that can be generated</param>
    ///<param name="iMax">The maximum value that can be generated</param>
    ///<return>Returns a random number between iMin and iMax.<return>
    int getRandomInt(int iMin, int iMax);

    ///<summary>
    ///Returns a random number between iMin (inclusive) and iMax (inclusive).
    ///</summary>
    ///<param name="iMin">The minimum value that can be generated</param>
    ///<param name="iMax">The maximum value that can be generated</param>
    ///<return>Returns a random number between iMin and iMax.<return>
    double getRandomDouble(double iMin, double iMax);

    ///<summary>
    ///Returns a random number between iMin (inclusive) and iMax (inclusive).
    ///</summary>
    ///<param name="iMin">The minimum value that can be generated</param>
    ///<param name="iMax">The maximum value that can be generated</param>
    ///<return>Returns a random number between iMin and iMax.<return>
    float getRandomFloat(float iMin, float iMax);

    ///<summary>
    ///Returns a random string.
    ///</summary>
    ///<return>Returns a random string.<return>
    std::string getRandomString();

    ///<summary>
    ///Returns a random string with a maximum length of iMaxLen.
    ///</summary>
    ///<param name="iMaxLen">The maximum length of the generated string.</param>
    ///<param name="oValue">The result of the generated string.</param>
    void getRandomString(std::string & oValue, size_t iMaxLen);
    std::string getRandomString(size_t iMaxLen);

    ///<summary>
    ///Returns a random string.
    ///</summary>
    ///<param name="iMaxLen">The maximum length of the generated string.</param>
    ///<param name="iSymbols">The list of allowed character of the generated string.</param>
    ///<param name="oValue">The result of the generated string.</param>
    void getRandomString(std::string & oValue, size_t iMaxLen, const char* iSymbols);
    std::string getRandomString(size_t iMaxLen, const char* iSymbols);

    class SymbolsFlags
    {
    public:
      enum Flags
      {
        Letters=1,            //lowercase letters
        LETTERS=2,            //uppercase letters
        Numbers=4,            //numbers from 0 to 9
        SpecialCharacters=8,  //special characters like @ and !
        All=15,               //all flags
      };
    };

    ///<summary>
    ///Returns a random string.
    ///</summary>
    ///<param name="iMaxLen">The maximum length of the generated string.</param>
    ///<param name="iFlags">The flags that defines the allowed content of the generated string.</param>
    ///<param name="oValue">The result of the generated string.</param>
    void getRandomString(std::string & oValue, size_t iMaxLen, SymbolsFlags::Flags iFlags);

  } //namespace random
} //namespace ra

#endif //RA_RANDOM_H
