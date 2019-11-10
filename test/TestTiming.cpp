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

#include "TestTiming.h"
#include "rapidassist/timing.h"

namespace ra { namespace timing { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestTiming::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestTiming::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testMillisleep) {
    //synchronize time to a new seconds
    WaitNextSecond();

    //assert that Millisleep() is actually sleeping
    std::tm time1 = GetLocalTime();
    ASSERT_EQ(0, ra::timing::Millisleep(3000 + 50)); //at least 3 seconds
    std::tm time2 = GetLocalTime();

    //convert hour, minute and seconds to absolute seconds
    int seconds1 = time1.tm_hour * 3600 + time1.tm_min * 60 + time1.tm_sec;
    int seconds2 = time2.tm_hour * 3600 + time2.tm_min * 60 + time2.tm_sec;

    static const int EXPECTED = 3;
    int diff = seconds2 - seconds1;

    //assert near
    ASSERT_GT(diff, EXPECTED - 1); //allow 1 seconds of difference
    ASSERT_LT(diff, EXPECTED + 1); //allow 1 seconds of difference
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetCopyrightYear) {
    int year = GetCopyrightYear();
    DateTime now = ToDateTime(GetLocalTime());
    ASSERT_GT(year, 2016); //assert value is not default value
    ASSERT_LE(year, now.year);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testDateTimeConvert) {
    std::tm now = GetLocalTime();
    DateTime dt = ToDateTime(now);
    std::tm actual = ToTimeInfo(dt);

    ASSERT_EQ(now.tm_year, actual.tm_year);
    ASSERT_EQ(now.tm_mon, actual.tm_mon);
    ASSERT_EQ(now.tm_mday, actual.tm_mday);
    ASSERT_EQ(now.tm_hour, actual.tm_hour);
    ASSERT_EQ(now.tm_min, actual.tm_min);
    ASSERT_EQ(now.tm_sec, actual.tm_sec);
    ASSERT_EQ(now.tm_wday, actual.tm_wday);
    ASSERT_EQ(now.tm_yday, actual.tm_yday);
    ASSERT_EQ(now.tm_isdst, actual.tm_isdst);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetUtcTime) {
    std::tm local = GetLocalTime();
    std::tm utc = GetUtcTime();

    ASSERT_NE(local.tm_hour, utc.tm_hour);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetMicrosecondsTimerPerformance) {
    //find the resolution of the GetMicrosecondsTimer() function
    for (size_t i = 0; i < 10; i++) {
      double time1 = GetMicrosecondsTimer();
      double time2 = time1;

      //loop until the returned value of GetMicrosecondsTimer() has changed
      while (time2 == time1)
        time2 = GetMicrosecondsTimer();

      double milliseconds = (time2 - time1)*1000.0;
      double microseconds = milliseconds * 1000.0;
      printf("%f microseconds, %f milliseconds\n", microseconds, milliseconds);

      //we expect a maximum of 100 microseconds resolution
      ASSERT_LT(microseconds, 100); //lower than 100us
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetMicrosecondsTimerAgaintsSleep) {
    double time1 = GetMicrosecondsTimer();
    ra::timing::Millisleep(800);
    double time2 = GetMicrosecondsTimer();

    double milliseconds = (time2 - time1)*1000.0;
    ASSERT_NEAR(800, milliseconds, 30); //Windows have ~15ms accuracy. Don't know about linux
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetMillisecondsTimerPerformance) {
    //find the resolution of the GetMillisecondsTimer() function
    for (size_t i = 0; i < 10; i++) {
      double time1 = GetMillisecondsTimer();
      double time2 = time1;

      //loop until the returned value of GetMillisecondsTimer() has changed
      while (time2 == time1)
        time2 = GetMillisecondsTimer();

      double milliseconds = (time2 - time1)*1000.0;
      double microseconds = milliseconds * 1000.0;
      printf("%f milliseconds, %f microseconds\n", milliseconds, microseconds);

      //Windows have ~15.6ms accuracy by default. The interrupt timer can be modified to get ~1ms accuracy.
      //Linux usually have 10ms accuracy.
      ASSERT_LE(milliseconds, 16);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testGetMillisecondsTimerAgaintsSleep) {
    double time1 = GetMillisecondsTimer();
    ra::timing::Millisleep(800);
    double time2 = GetMillisecondsTimer();

    double milliseconds = (time2 - time1)*1000.0;
    ASSERT_NEAR(800, milliseconds, 30); //Windows have ~15ms accuracy. Don't know about linux
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace timing
} //namespace ra
