#include "TestRandom.h"
#include "random.h"

namespace ra { namespace random { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestRandom::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestRandom::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestRandom, testGetRandomInt)
  {
    int previous = 0;
    for(size_t i=0; i<100; i++)
    {
      int newValue = getRandomInt();
      ASSERT_NE(previous, newValue);
      previous = newValue;
    }
  }
  //--------------------------------------------------------------------------------------------------
  bool isAllTrue(bool * iArray, size_t iSize)
  {
    bool all = true;
    for(size_t i=0; i<iSize; i++)
    {
      if (iArray[i] == false)
        return false;
    }
    return true;
  }
  TEST_F(TestRandom, testGetRandomIntRange)
  {
    //assert that 10000 random number between -50 and +49 should go through all possible values
    {
      static const int RANGE_MIN = -50;
      static const int RANGE_MAX = +49;
      bool found[100] = {0};
      for(size_t i=0; i<10000; i++)
      {
        int value = getRandomInt(RANGE_MIN, RANGE_MAX);
        ASSERT_GE(value, RANGE_MIN);
        ASSERT_LE(value, RANGE_MAX);

        int index = value+50;
        ASSERT_GE(index,  0);
        ASSERT_LE(index, 99);

        found[index] = true;
        if (isAllTrue(found, sizeof(found)))
        {
          break;
        }
      }

      ASSERT_TRUE( isAllTrue(found, sizeof(found)) );
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestRandom, testGetRandomString)
  {
    ASSERT_TRUE( !getRandomString().empty() );

    ASSERT_TRUE( getRandomString(0).empty() );

    //assert maxlen
    for(int i=0; i<10; i++)
    {
      std::string s = getRandomString(i);
      ASSERT_EQ(i, (int)s.size());
    }

    //assert symbols
    static const std::string SYMBOLS = "0123abc";
    for(int i=0; i<1000; i++)
    {
      std::string s = getRandomString(20, SYMBOLS.c_str());
      ASSERT_EQ(20, (int)s.size());

      for(size_t j=0; j<s.size(); j++)
      {
        ASSERT_TRUE(
          s[j] == '0' ||
          s[j] == '1' ||
          s[j] == '2' ||
          s[j] == '3' ||
          s[j] == 'a' ||
          s[j] == 'b' ||
          s[j] == 'c'     );
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace random
} //namespace ra
