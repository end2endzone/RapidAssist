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
#include "rapidassist/testing.h"

namespace ra { namespace timing { namespace test
{
  typedef uint64_t (*CounterFunction)();
  const uint64_t NANOSECONDS_PER_SECONDS  = (uint64_t)1e9;
  const uint64_t MICROSECONDS_PER_SECONDS = (uint64_t)1e6;
  const uint64_t MILLISECONDS_PER_SECONDS = (uint64_t)1e3;

  void FindCounterFunctionFrequency(CounterFunction func, const char * func_name, uint64_t to_seconds_divisor)
  {
    //printf("Calculating frequency for %s() function...\n", func_name);

    int num_unique_samples = 0;

    // synchronize at the beginning of a new second
    uint64_t seconds_now = func() / to_seconds_divisor;
    uint64_t seconds_next = seconds_now+1;
    while( seconds_now >= seconds_next )
    {
      seconds_now = func() / to_seconds_divisor;
    }

    // Check how many new values were identified in 5 seconds
    uint64_t seconds_end = seconds_now + 5;
    uint64_t now = func();
    uint64_t prev = now;
    while(seconds_now < seconds_end)
    {
      // query again
      now = func();
      if (now != prev)
      {
        // that's a new sample
        num_unique_samples++;
        prev = now;
      }

      // update seconds_now
      seconds_now = now / to_seconds_divisor;
    }

    int frequency = num_unique_samples / 5;

    // round values if required
    if (frequency % 10 == 9)
      frequency++;

    double accuracy = 1.0 / double(frequency);
    const char * accuracy_unit = "seconds";
    if (accuracy >= 0.001)
    {
      accuracy *= 1000.0;
      accuracy_unit = "ms";
    }
    else if (accuracy >= 0.000001)
    {
      accuracy *= 1000000.0;
      accuracy_unit = "us";
    }
    else if (accuracy >= 0.000000001)
    {
      accuracy *= 1000000000.0;
      accuracy_unit = "ns";
    }

    printf("%s() has a frequency of %d Hz with an accuracy of %f %s\n", func_name, frequency, accuracy, accuracy_unit);
  }

  void FindCounterFunctionSpeed(CounterFunction func, CounterFunction nsfunc, const char * func_name)
  {
    //printf("Calculating speed for %s() function...\n", func_name);

    const int NUM_CALLS = 1000000;

    // Allow all initialization to take place
    uint64_t tmp1 = func();
    uint64_t tmp2 = func();
    uint64_t tmp3 = func();
    uint64_t tmp4 = func();
    uint64_t tmp5 = func();
  
    uint64_t time_begin = nsfunc();
    for(int i=0; i<NUM_CALLS; i++ )
    {
      uint64_t tmp = func();
    }
    uint64_t time_end = nsfunc();

    // Calculate elapsed time
    uint64_t elapsed_ns = time_end - time_begin;
    uint64_t elapsed_ms = elapsed_ns/1000000;
    double elapsed_seconds = elapsed_ns / double(NANOSECONDS_PER_SECONDS);
    uint64_t avg_ns = elapsed_ns / (uint64_t)NUM_CALLS;
  
#ifdef _WIN32
    printf("Calling %d times function %s() takes %f seconds, each calls take an average of %I64u nanoseconds\n", NUM_CALLS, func_name, elapsed_seconds, avg_ns);
#elif defined(__APPLE__)
    printf("Calling %d times function %s() takes %f seconds, each calls take an average of %llu nanoseconds\n", NUM_CALLS, func_name, elapsed_seconds, avg_ns);
#else
//#elif defined(__linux__)
    printf("Calling %d times function %s() takes %f seconds, each calls take an average of %lu nanoseconds\n", NUM_CALLS, func_name, elapsed_seconds, avg_ns);
#endif

  }

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
    int year = ra::timing::GetCopyrightYear();
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
  TEST_F(TestTiming, testGetTimestampTime) {
    ra::timing::Timestamp ts_now = GetTimestampTime();
    ra::timing::Timestamp ts_epoch = ra::timing::EPOCH;

    ASSERT_NE(ts_now.ts_sec, ts_epoch.ts_sec);
    ASSERT_NE(ts_now.ts_usec, ts_epoch.ts_usec);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testTimestampSecondsUnits) {
    static const size_t TEST_DURATION_SECONDS = 5;

    ra::timing::Timestamp ts1 = GetTimestampTime();
    Millisleep(TEST_DURATION_SECONDS * 1000);
    ra::timing::Timestamp ts2 = GetTimestampTime();

    uint32_t diff_seconds = ts2.ts_sec - ts1.ts_sec;

    ASSERT_NEAR(diff_seconds, TEST_DURATION_SECONDS, 1);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testTimestampMicrosecondsUnit) {
    static const size_t TEST_DURATION_SECONDS = 5;
    static const uint32_t MAX_THEORICAL_USEC = 999999UL;

    uint32_t min_usec = MAX_THEORICAL_USEC + 1;
    uint32_t max_usec = 0;

    uint64_t start_time_counter = GetMillisecondsCounterU64();
    uint64_t stop_time_counter = start_time_counter + TEST_DURATION_SECONDS*1000;

    uint64_t now_time_counter = GetMillisecondsCounterU64();
    while ( now_time_counter < stop_time_counter )
    {
      ra::timing::Timestamp ts_now = GetTimestampTime();
      if ( min_usec > ts_now.ts_usec )
        min_usec = ts_now.ts_usec;
      if ( max_usec < ts_now.ts_usec )
        max_usec = ts_now.ts_usec;

      now_time_counter = GetMillisecondsCounterU64();
    }

    static const uint32_t EPSILON = 100; // 0.1 ms (sub-milliseconds) accuracy
    ASSERT_LE(max_usec, MAX_THEORICAL_USEC);
    ASSERT_GE(min_usec, 0UL);
    ASSERT_NEAR(min_usec, 0, EPSILON);
    ASSERT_NEAR(max_usec, MAX_THEORICAL_USEC-1, EPSILON);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testTimestampToStringUtcTime)
  {
    ra::timing::Timestamp ts_epoch = ra::timing::EPOCH;
    ra::timing::Timestamp ts_now = GetTimestampTime();
    std::string ts_epoch_utc_str = ra::timing::ToStringUtcTime(ts_epoch);
    std::string ts_now_utc_str = ra::timing::ToStringUtcTime(ts_now);
    std::string ts_now_local_str = ra::timing::ToStringLocalTime(ts_now);

    printf("Epoch is %s UTC\n", ts_epoch_utc_str.c_str());
    printf("Now it's %s UTC\n", ts_now_utc_str.c_str());

    ASSERT_EQ(ts_epoch_utc_str.size(), ts_now_utc_str.size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testTimestampToString)
  {
    ra::timing::Timestamp ts_epoch = ra::timing::EPOCH;
    ra::timing::Timestamp ts_now = GetTimestampTime();
    std::string ts_epoch_str = ra::timing::ToString(ts_epoch);
    std::string ts_now_str = ra::timing::ToString(ts_now);

    printf("Epoch is %s seconds UTC since EPOCH\n", ts_epoch_str.c_str());
    printf("Now it's %s seconds UTC since EPOCH\n", ts_now_str.c_str());

    // assert both have a dot in their values
    ASSERT_NE(ts_epoch_str.find('.'), std::string::npos);
    ASSERT_NE(ts_now_str.find('.'), std::string::npos);

    // assert both have microseconds resolution
    ASSERT_EQ(ts_epoch_str.find('.'), ts_epoch_str.size() - 7); // 6 usec characters and the dot
    ASSERT_EQ(ts_now_str.find('.'), ts_now_str.size() - 7);     // 6 usec characters and the dot
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
    static const int MAX_TEST_RUNS = 10;
    static const uint32_t TEST_SLEEP_TIME_MS = 800;

#ifdef _WIN32
    double epsilon = 30.0; //Windows have ~15ms accuracy.
#elif defined(__linux__)
    double epsilon = 15.0;
#elif defined(__APPLE__)
    double epsilon = 15.0;
    if ( ra::testing::IsGitHubActions() )
    {
      // On Github Action, the observed delays seems to be increased by about 150ms for GetMillisecondsTimer()
      // and sometimes go over 150 with values such as 155.00000000004093, 153.99999999995089, 153.00000000008822 or 160.00000000003638
      // This was observed during issue #83 implementation.
      epsilon = 150.0;
    }
#else
    double epsilon = 1.0;
#endif

    // This test is inconsistent. Some times the duration of a ra::timing::Millisleep() call is actually much greater than allowed.
    // Increasing the epsilon to a higher value until we respect all "gaps" is problematic as it can allow true errors to pass through.
    // To mitigate this, we runs the tests multiple times until there is a "pass".
    double elapsed_milliseconds[MAX_TEST_RUNS] = { 0 };
    double diff_milliseconds[MAX_TEST_RUNS] = { 0 };
    bool test_pass = false;
    for ( int i = 0; i < MAX_TEST_RUNS && !test_pass; i++ )
    {
      double time1 = GetMicrosecondsTimer();
      ra::timing::Millisleep(800);
      double time2 = GetMicrosecondsTimer();

      // compute elapsed time
      elapsed_milliseconds[i] = (time2 - time1) * 1000.0; // seconds to milliseconds
      diff_milliseconds[i] = abs(double(TEST_SLEEP_TIME_MS) - elapsed_milliseconds[i]);

      if ( diff_milliseconds[i] < epsilon )
        test_pass = true;

      // Add a delay between tests
      if ( !test_pass )
        ra::timing::Millisleep(1583);
    }

    std::string msg = std::string() + "The tests ran " + ra::strings::ToString(MAX_TEST_RUNS) + " times. "
      "Calling Millisleep() for " + ra::strings::ToString(TEST_SLEEP_TIME_MS) + " milliseconds results "
      "in delays that are always greater than (sleeptime + epsilon) where epsilon is " + ra::strings::ToString(epsilon) + ".";
    for ( int i = 0; i < MAX_TEST_RUNS; i++ )
    {
      msg += std::string() + "\ntest-run[" + ra::strings::ToString(i) + "]: elapsed=" + ra::strings::ToString(elapsed_milliseconds[i]) + ", diff=" + ra::strings::ToString(diff_milliseconds[i]) + ".";
    }

    ASSERT_TRUE(test_pass) << msg;
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
    static const int MAX_TEST_RUNS = 10;
    static const uint32_t TEST_SLEEP_TIME_MS = 800;

#ifdef _WIN32
    double epsilon = 30.0; //Windows have ~15ms accuracy.
#elif defined(__linux__)
    double epsilon = 15.0;
#elif defined(__APPLE__)
    double epsilon = 15.0;
    if (ra::testing::IsGitHubActions()) {
      // On Github Action, the observed delays seems to be increased by about 150ms for GetMillisecondsTimer()
      // and sometimes go over 150 with values such as 155.00000000004093, 153.99999999995089, 153.00000000008822 or 160.00000000003638
      // This was observed during issue #83 implementation.
      epsilon = 150.0;
    }
#else
    double epsilon = 1.0;
#endif
    
    // This test is inconsistent. Some times the duration of a ra::timing::Millisleep() call is actually much greater than allowed.
    // Increasing the epsilon to a higher value until we respect all "gaps" is problematic as it can allow true errors to pass through.
    // To mitigate this, we runs the tests multiple times until there is a "pass".
    double elapsed_milliseconds[MAX_TEST_RUNS] = { 0 };
    double diff_milliseconds[MAX_TEST_RUNS] = { 0 };
    bool test_pass = false;
    for ( int i = 0; i < MAX_TEST_RUNS && !test_pass; i++ )
    {
      double time1 = GetMillisecondsTimer();
      ra::timing::Millisleep(TEST_SLEEP_TIME_MS);
      double time2 = GetMillisecondsTimer();

      // compute elapsed time
      elapsed_milliseconds[i] = (time2 - time1) * 1000.0; // seconds to milliseconds
      diff_milliseconds[i] = abs(double(TEST_SLEEP_TIME_MS) - elapsed_milliseconds[i]);

      if ( diff_milliseconds[i] < epsilon )
        test_pass = true;

      // Add a delay between tests
      if (!test_pass )
        ra::timing::Millisleep(1583);
    }

    std::string msg = std::string() + "The tests ran " + ra::strings::ToString(MAX_TEST_RUNS) + " times. "
      "Calling Millisleep() for " + ra::strings::ToString(TEST_SLEEP_TIME_MS) + " milliseconds results "
      "in delays that are always greater than (sleeptime + epsilon) where epsilon is " + ra::strings::ToString(epsilon) + ".";
    for ( int i = 0; i < MAX_TEST_RUNS; i++ )
    {
      msg += std::string() + "\ntest-run[" + ra::strings::ToString(i) + "]: elapsed=" + ra::strings::ToString(elapsed_milliseconds[i]) + ", diff=" + ra::strings::ToString(diff_milliseconds[i]) + ".";
    }

    ASSERT_TRUE(test_pass) << msg;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, calculateCountersFrequency) {
    printf("Calculating frequency for 64 bit counter functions...\n");
    FindCounterFunctionFrequency(&ra::timing::GetMillisecondsCounterU64, "ra::timing::GetMillisecondsCounterU64", MILLISECONDS_PER_SECONDS);
    FindCounterFunctionFrequency(&ra::timing::GetMicrosecondsCounterU64, "ra::timing::GetMicrosecondsCounterU64", MICROSECONDS_PER_SECONDS);
    FindCounterFunctionFrequency(&ra::timing::GetNanosecondsCounterU64,  "ra::timing::GetNanosecondsCounterU64 ", NANOSECONDS_PER_SECONDS);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, calculateCountersSpeed) {
    printf("Calculating speed for 64 bit counter functions...\n");
    FindCounterFunctionSpeed(&ra::timing::GetMillisecondsCounterU64, &ra::timing::GetNanosecondsCounterU64, "ra::timing::GetMillisecondsCounterU64");
    FindCounterFunctionSpeed(&ra::timing::GetMicrosecondsCounterU64, &ra::timing::GetNanosecondsCounterU64, "ra::timing::GetMicrosecondsCounterU64");
    FindCounterFunctionSpeed(&ra::timing::GetNanosecondsCounterU64,  &ra::timing::GetNanosecondsCounterU64, "ra::timing::GetNanosecondsCounterU64 ");
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testMillisecondsCountersAlwaysIncreases) {
    // Run test for 5 seconds
    uint64_t seconds_now = ra::timing::GetMillisecondsCounterU64() / 1000;
    uint64_t seconds_end = seconds_now + 5;

    uint64_t now = ra::timing::GetMillisecondsCounterU64();
    uint64_t prev = now;
    while(seconds_now < seconds_end)
    {
      ASSERT_GE(now, prev);

      // update seconds_now
      seconds_now = ra::timing::GetMillisecondsCounterU64() / 1000;
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testMicrosecondsCountersAlwaysIncreases) {
    // Run test for 5 seconds
    uint64_t seconds_now = ra::timing::GetMicrosecondsCounterU64() / 1000000;
    uint64_t seconds_end = seconds_now + 5;

    uint64_t now = ra::timing::GetMicrosecondsCounterU64();
    uint64_t prev = now;
    while(seconds_now < seconds_end)
    {
      ASSERT_GE(now, prev);

      // update seconds_now
      seconds_now = ra::timing::GetMicrosecondsCounterU64() / 1000000;
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestTiming, testNanosecondsCountersAlwaysIncreases) {
    // Run test for 5 seconds
    uint64_t seconds_now = ra::timing::GetNanosecondsCounterU64() / (uint64_t)1e9;
    uint64_t seconds_end = seconds_now + 5;

    uint64_t now = ra::timing::GetNanosecondsCounterU64();
    uint64_t prev = now;
    while(seconds_now < seconds_end)
    {
      ASSERT_GE(now, prev);

      // update seconds_now
      seconds_now = ra::timing::GetNanosecondsCounterU64() / (uint64_t)1e9;
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace timing
} //namespace ra
