#include "TestTime.h"
#include "time_.h"

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
    std::tm time1 = getLocalTime();
    ASSERT_EQ(0, time::millisleep(5000 + 50)); //at least 5 seconds
    std::tm time2 = getLocalTime();

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
  TEST_F(TestTime, testGetCopyrightYear)
  {
    int year = getCopyrightYear();
    DATETIME now = toDateTime(getLocalTime());
    ASSERT_GT(year, 2016); //assert value is not default value
    ASSERT_LE(year, now.year);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTime, testDateTimeConvert)
  {
    std::tm now = getLocalTime();
    DATETIME dt = toDateTime(now);
    std::tm actual = toTimeInfo(dt);

    ASSERT_EQ(now.tm_year , actual.tm_year );
    ASSERT_EQ(now.tm_mon  , actual.tm_mon  );
    ASSERT_EQ(now.tm_mday , actual.tm_mday );
    ASSERT_EQ(now.tm_hour , actual.tm_hour );
    ASSERT_EQ(now.tm_min  , actual.tm_min  );
    ASSERT_EQ(now.tm_sec  , actual.tm_sec  );
    ASSERT_EQ(now.tm_wday , actual.tm_wday );
    ASSERT_EQ(now.tm_yday , actual.tm_yday );
    ASSERT_EQ(now.tm_isdst, actual.tm_isdst);
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace time
} //namespace ra
