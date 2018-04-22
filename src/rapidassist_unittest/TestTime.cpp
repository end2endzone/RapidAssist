#include "TestTime.h"
#include "ratime.h"

namespace ra { namespace time { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestTime::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestTime::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTime, testMillisleep)
  {
    //synchronize time to a new seconds
    waitNextSecond();

    //assert that millisleep() is actually sleeping
    std::tm time1 = getLocalSystemTime();
    ASSERT_EQ(0, time::millisleep(5000 + 50)); //at least 5 seconds
    std::tm time2 = getLocalSystemTime();

    //convert hour, minute and seconds to absolute seconds
    int seconds1 = time1.tm_hour*3600+time1.tm_min*60+time1.tm_sec;
    int seconds2 = time2.tm_hour*3600+time2.tm_min*60+time2.tm_sec;
    
    static const int EXPECTED = 5;
    int diff = seconds2 - seconds1;

    //assert near
    ASSERT_GT(diff, EXPECTED-1); //allow 1 seconds of difference
    ASSERT_LT(diff, EXPECTED+1); //allow 1 seconds of difference
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace time
} //namespace ra
