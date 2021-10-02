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

#include "rapidassist/timing.h"

#include <cstdlib> //for atoi()

#if defined(__APPLE__)
  //https://github.com/envoyproxy/envoy/issues/1789
  //https://boringssl.googlesource.com/boringssl/+/63a0797ff247f13870b649c3f6239d80be202752
  #define _DARWIN_C_SOURCE
#endif

//https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
#ifdef __MACH__
#include <mach/mach_time.h> /* mach_absolute_time */
#include <mach/mach.h>      /* host_get_clock_service, mach_... */
#include <mach/clock.h>     /* clock_get_time */
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h> // for Sleep()
#include "rapidassist/undef_windows_macros.h"
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep()
#else
#include <unistd.h> // for usleep()
#endif

#ifdef _WIN32
#include <Mmsystem.h> //for timeGetTime(), timeBeginPeriod()
#pragma comment(lib, "winmm.lib") //for timeGetTime(), timeBeginPeriod()
#else
#include <sys/time.h> //for clock_gettime()
#endif

#include <time.h> //for time() and localtime()
#include <string>

namespace ra { namespace timing {

  const uint64_t NANOSECONDS_PER_SECONDS = (uint64_t)1e9;

#if defined(__linux__) || defined(__APPLE__)
  //linux:
  //https://stackoverflow.com/questions/12392278/measure-time-in-linux-time-vs-clock-vs-getrusage-vs-clock-gettime-vs-gettimeof
  //http://nadeausoftware.com/articles/2012/04/c_c_tip_how_measure_elapsed_real_time_benchmarking
  //POSIX clocks benchmark: https://stackoverflow.com/a/13096917

  //mach:
  //https://gist.github.com/jbenet/1087739
  //https://stackoverflow.com/questions/21665641/ns-precision-monotonic-clock-in-c-on-linux-and-os-x/21665642#21665642
#endif

#ifdef _WIN32
  //
  //Multiple timing solutions for Windows based systems.
  //See the following for details:
  //  http://nadeausoftware.com/articles/2012/04/c_c_tip_how_measure_elapsed_real_time_benchmarking
  //  http://www.windowstimestamp.com/description
  //  https://gamedev.stackexchange.com/questions/26759/best-way-to-get-elapsed-time-in-miliseconds-in-windows
  //

  ///<summary>
  ///Returns the elasped time in seconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have below microsecond accuracy.
  ///  - Use the QueryPerformanceCounter, affected by multicore processors.
  ///  - Must be called from the same thread to compute elapsed time.
  ///
  /// Warning for processes running a multicore processors...
  /// While QueryPerformanceCounter and QueryPerformanceFrequency typically adjust
  /// for multiple processors, bugs in the BIOS or drivers may result in these routines
  /// returning different values as the thread moves from one processor to another.
  /// So, it's best to keep the thread on a single processor.
  /// See the following for details:
  ///   https://msdn.microsoft.com/en-us/library/windows/desktop/ee417693(v=vs.85).aspx
  ///   https://stackoverflow.com/questions/44020619/queryperformancecounter-on-multi-core-processor-under-windows-10-behaves-erratic
  ///   https://msdn.microsoft.com/en-us/library/windows/desktop/ms686247(v=vs.85).aspx (SetThreadAffinityMask function)
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point. Returns 0 on error.</returns>
  inline double GetSecondsFromPerformanceTimer() {
    //Get frequency in counts per seconds.
    static LARGE_INTEGER frequency = { 0 };
    if (frequency.QuadPart == 0)
    {
      if (!QueryPerformanceFrequency(&frequency))
        return 0.0;
    }

    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter))
      return 0.0;
    double seconds = counter.QuadPart / double(frequency.QuadPart);
    return seconds;
  }

  ///<summary>
  ///Initialize the multimedia timer to have a 1 millisecond resolution.
  ///By default, the resolution is 15.6ms (64Hz).
  ///The function must be called once per process execution.
  ///</summary>
  inline void InitMillisecondsInterruptTimer() {
    //allow running only once
    static bool firstPass = true;
    if (!firstPass)
      return;
    firstPass = false;

    //force 1ms resolution, default resolution is ~15ms
    timeBeginPeriod(1);

    //wait for the system to apply the new period
    DWORD diff = 0;
    while (diff == 0 || diff > 5) {
      DWORD time1 = timeGetTime();
      DWORD time2 = timeGetTime();
      diff = time2 - time1;
    }
  }

  ///<summary>
  ///Returns the elasped time in seconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have a fixed ~15.6ms accuracy.
  ///  - Does not requires the multimedia timer initialization.
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  inline double GetSecondsFromGetTickCount() //fast constant 15ms timer
  {
    DWORD milliseconds_counter = GetTickCount();
    double seconds = double(milliseconds_counter) / 1000.0;
    return seconds;
  }

  ///<summary>
  ///Returns the elasped time in seconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have 1ms accuracy.
  ///  - Uses the multimedia timer. See also InitMillisecondsInterruptTimer().
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  inline double GetSecondsFromTimeGetTime() {
    DWORD milliseconds_counter = timeGetTime();
    double seconds = double(milliseconds_counter) / 1000.0;
    return seconds;
  }

  //Find if GetSystemTimePreciseAsFileTime() function is available on current system
  // https://stackoverflow.com/questions/24241916/how-to-check-win-api-function-support-during-runtime
  typedef void (WINAPI *FuncT) (LPFILETIME lpSystemTimeAsFileTime);
  HINSTANCE hKernelDll = LoadLibrary(TEXT("Kernel32.dll"));
  FuncT GetSystemTimePreciseAsFileTime_ = (FuncT)GetProcAddress((HMODULE)hKernelDll, "GetSystemTimePreciseAsFileTime");

  ///<summary>
  ///Returns the elasped time in seconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have ~1ms accuracy on Windows 7 and before.
  ///  - Have <1us accuracy on Windows 8 and up.
  ///  - Uses the multimedia timer. See also InitMillisecondsInterruptTimer().
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  inline double GetSecondsFromSystemTime() {
    FILETIME file_time;
    ULONGLONG t;
    if (GetSystemTimePreciseAsFileTime_) {
      //Windows 8, Windows Server 2012 and later
      GetSystemTimePreciseAsFileTime_(&file_time); //microseconds accuracy
    }
    else {
      //Windows 2000 and later
      GetSystemTimeAsFileTime(&file_time); //milliseconds accuracy
    }
    t = ((ULONGLONG)file_time.dwHighDateTime << 32) | (ULONGLONG)file_time.dwLowDateTime;
    return (double)t / 10000000.0;
  }

  ///<summary>
  ///Returns the elasped time in nanoseconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have below microsecond accuracy.
  ///  - Use the QueryPerformanceCounter, affected by multicore processors.
  ///  - Must be called from the same thread to compute elapsed time.
  ///
  /// Warning for processes running a multicore processors...
  /// While QueryPerformanceCounter and QueryPerformanceFrequency typically adjust
  /// for multiple processors, bugs in the BIOS or drivers may result in these routines
  /// returning different values as the thread moves from one processor to another.
  /// So, it's best to keep the thread on a single processor.
  /// See the following for details:
  ///   https://msdn.microsoft.com/en-us/library/windows/desktop/ee417693(v=vs.85).aspx
  ///   https://stackoverflow.com/questions/44020619/queryperformancecounter-on-multi-core-processor-under-windows-10-behaves-erratic
  ///   https://msdn.microsoft.com/en-us/library/windows/desktop/ms686247(v=vs.85).aspx (SetThreadAffinityMask function)
  ///</remarks>
  ///<returns>Returns the elapsed time in nanoseconds since an arbitrary starting point. Returns 0 on error.</returns>
  inline uint64_t GetNanosecondsCounterFromPerformanceTimer() {
    //Get frequency in counts per seconds.
    static LARGE_INTEGER frequency = { 0 };
    if (frequency.QuadPart == 0)
    {
      if (!QueryPerformanceFrequency(&frequency))
        return 0;
    }

    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter))
      return 0;

    uint64_t nanoseconds = (uint64_t) ((NANOSECONDS_PER_SECONDS * counter.QuadPart) / frequency.QuadPart);
    return nanoseconds;
  }

  ///<summary>
  ///Returns the elasped time in nanoseconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have ~1ms accuracy on Windows 7 and before.
  ///  - Have <1us accuracy on Windows 8 and up.
  ///  - Uses the multimedia timer. See also InitMillisecondsInterruptTimer().
  ///</remarks>
  ///<returns>Returns the elapsed time in nanoseconds since an arbitrary starting point.</returns>
  inline uint64_t GetNanosecondsCounterFromSystemTime() {
    FILETIME file_time;
    if (GetSystemTimePreciseAsFileTime_) {
      //Windows 8, Windows Server 2012 and later
      GetSystemTimePreciseAsFileTime_(&file_time); //microseconds accuracy
    }
    else {
      //Windows 2000 and later
      GetSystemTimeAsFileTime(&file_time); //milliseconds accuracy
    }
    uint64_t nanoseconds = ((uint64_t)file_time.dwHighDateTime << 32) | (uint64_t)file_time.dwLowDateTime;
    nanoseconds *= 100;
    return nanoseconds;
  }

  ///<summary>
  ///Returns the elasped time in nanoseconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have 15ms accuracy by default.
  ///  - Have  1ms accuracy if after calling InitMillisecondsInterruptTimer().
  ///  - Uses the multimedia timer.
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  inline uint64_t GetNanosecondsCounterFromTimeGetTime() {
    DWORD ms = timeGetTime();
    uint64_t ns = (uint64_t)ms*1000*1000;
    return ns;
  }

  ///<summary>
  ///Returns the elasped time in nanoseconds since an arbitrary starting point.
  ///</summary>
  ///<remarks>
  ///The function have the following properties:
  ///  - Have a fixed ~15.6ms accuracy.
  ///  - Does not requires the multimedia timer initialization.
  ///</remarks>
  ///<returns>Returns the elapsed time in nanoseconds since an arbitrary starting point.</returns>
  inline uint64_t GetNanosecondsCounterFromGetTickCount()
  {
    uint32_t ms = GetTickCount(); //fast constant 15.6ms timer
    uint64_t ns = (uint64_t)ms*1000*1000;
    return ns;
  }

#endif

#ifdef __linux__
  double GetMicrosecondsTimerFromMonotonicTimer() {
    //Using CLOCK_MONOTONIC_RAW because timer is not adjusted by adjtime/NTP.
    //We won't risk having a frequency adjustement while the process is running.
    //See the following for details:
    //  https://stackoverflow.com/questions/25583498/clock-monotonic-vs-clock-monotonic-raw-truncated-values
    //  https://stackoverflow.com/questions/14270300/what-is-the-difference-between-clock-monotonic-clock-monotonic-raw
    //  https://stackoverflow.com/questions/31073923/clockid-t-clock-gettime-first-argument-portability
    struct timespec now;
    clockid_t clock_id = 0;

    //use the best clock available on the system.
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) == 0) { clock_id = CLOCK_MONOTONIC_RAW; }
    else if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) { clock_id = CLOCK_MONOTONIC; }
    else if (clock_gettime(CLOCK_REALTIME, &now) == 0) { clock_id = CLOCK_REALTIME; }
    else {
      //all calls to clock_gettime() have failed.
      //fallback to gettimeofday()
      struct timeval tm;
      gettimeofday(&tm, NULL);
      double seconds = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
      return seconds;
    }

    //clock_gettime() is successful
    double seconds = now.tv_sec + now.tv_nsec / 1000000000.0;
    return seconds;
  }
  
  double GetMicrosecondsTimerFromMonotonicCoarse() {
    struct timespec now;
    clockid_t clock_id = 0;

    //use a coarse clock available on the system.
    if (clock_gettime(CLOCK_MONOTONIC_COARSE, &now) == 0) { clock_id = CLOCK_MONOTONIC_COARSE; }
    else if (clock_gettime(CLOCK_REALTIME_COARSE, &now) == 0) { clock_id = CLOCK_REALTIME_COARSE; }
    else if (clock_gettime(CLOCK_REALTIME, &now) == 0) { clock_id = CLOCK_REALTIME; }
    else {
      //all calls to clock_gettime() have failed.
      //fallback to gettimeofday()
      struct timeval tm;
      gettimeofday(&tm, NULL);
      double seconds = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
      return seconds;
    }

    //clock_gettime() is successful
    double seconds = now.tv_sec + now.tv_nsec / 1000000000.0;
    return seconds;
  }
  
  uint64_t GetNanosecondsCounterFromMonotonicTimer() {
    //Using CLOCK_MONOTONIC_RAW because timer is not adjusted by adjtime/NTP.
    //We won't risk having a frequency adjustement while the process is running.
    //See the following for details:
    //  https://stackoverflow.com/questions/25583498/clock-monotonic-vs-clock-monotonic-raw-truncated-values
    //  https://stackoverflow.com/questions/14270300/what-is-the-difference-between-clock-monotonic-clock-monotonic-raw
    //  https://stackoverflow.com/questions/31073923/clockid-t-clock-gettime-first-argument-portability
    struct timespec now;
    clockid_t clock_id = 0;

    //use the best clock available on the system.
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) == 0) { clock_id = CLOCK_MONOTONIC_RAW; }
    else if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) { clock_id = CLOCK_MONOTONIC; }
    else if (clock_gettime(CLOCK_REALTIME, &now) == 0) { clock_id = CLOCK_REALTIME; }
    else {
      //all calls to clock_gettime() have failed.
      //fallback to gettimeofday()
      struct timeval tm;
      gettimeofday(&tm, NULL);
      uint64_t nanoseconds = ((uint64_t)tm.tv_sec)*NANOSECONDS_PER_SECONDS + ((uint64_t)tm.tv_usec)*1000;
      return nanoseconds;
    }

    //clock_gettime() is successful
    uint64_t nanoseconds = ((uint64_t)now.tv_sec)*NANOSECONDS_PER_SECONDS + (uint64_t)now.tv_nsec;
    return nanoseconds;
  }
  
  uint64_t GetNanosecondsCounterFromMonotonicCoarse() {
    struct timespec now;
    clockid_t clock_id = 0;

    //use a coarse clock available on the system.
    if (clock_gettime(CLOCK_MONOTONIC_COARSE, &now) == 0) { clock_id = CLOCK_MONOTONIC_COARSE; }
    else if (clock_gettime(CLOCK_REALTIME_COARSE, &now) == 0) { clock_id = CLOCK_REALTIME_COARSE; }
    else if (clock_gettime(CLOCK_REALTIME, &now) == 0) { clock_id = CLOCK_REALTIME; }
    else {
      //all calls to clock_gettime() have failed.
      //fallback to gettimeofday()
      struct timeval tm;
      gettimeofday(&tm, NULL);
      uint64_t nanoseconds = ((uint64_t)tm.tv_sec)*NANOSECONDS_PER_SECONDS + ((uint64_t)tm.tv_usec)*1000;
      return nanoseconds;
    }

    //clock_gettime() is successful
    uint64_t nanoseconds = ((uint64_t)now.tv_sec)*NANOSECONDS_PER_SECONDS + (uint64_t)now.tv_nsec;
    return nanoseconds;
  }

#endif


#ifdef __APPLE__
  double GetMicrosecondsTimerFromCalendarClock() {
    //https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
    //https://developer.apple.com/documentation/kernel/1462446-mach_absolute_time
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    struct timespec ts;
    
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    
    double seconds = ts.tv_sec + ts.tv_nsec / 1000000000.0;
    return seconds;
  }
  
  double GetMicrosecondsTimerFromSystemClock() {
    //https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
    //https://developer.apple.com/documentation/kernel/1462446-mach_absolute_time
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    struct timespec ts;
    
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    
    double seconds = ts.tv_sec + ts.tv_nsec / 1000000000.0;
    return seconds;
  }
  
  double GetMicrosecondsTimerFromMachAbsTime() {
    //https://stackoverflow.com/questions/41509505/clock-gettime-on-macos
    //https://developer.apple.com/library/archive/qa/qa1398/_index.html
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    mach_timebase_info_data_t tbi;
    mach_timebase_info(&tbi);

    // Get absolute time in unknow units of time
    uint64_t base_time = mach_absolute_time();

    // Convert to nanoseconds
    uint64_t elapsed_nano = base_time * tbi.numer / tbi.denom;
    uint64_t elapsed_micro = elapsed_nano / 1000;
    
    double seconds = elapsed_micro / 1000000.0; // microseconds to seconds
    return seconds;
  }
  
  double GetMillisecondsTimerFromMachAbsTime() {
    //https://stackoverflow.com/questions/41509505/clock-gettime-on-macos
    //https://developer.apple.com/library/archive/qa/qa1398/_index.html
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    mach_timebase_info_data_t tbi;
    mach_timebase_info(&tbi);

    // Get absolute time in unknow units of time
    uint64_t base_time = mach_absolute_time();

    // Convert to nanoseconds
    uint64_t elapsed_nano = base_time * tbi.numer / tbi.denom;
    uint64_t elapsed_milli = elapsed_nano / 1000000;
    
    double seconds = elapsed_milli / 1000.0; // milliseconds to seconds
    return seconds;
  }

  uint64_t GetNanosecondsCounterFromCalendarClock() {
    //https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
    //https://developer.apple.com/documentation/kernel/1462446-mach_absolute_time
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    struct timespec ts;
    
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    
    uint64_t nanoseconds = ((uint64_t)ts.tv_sec)*NANOSECONDS_PER_SECONDS + (uint64_t)ts.tv_nsec;
    return nanoseconds;
  }
  
  uint64_t GetNanosecondsCounterFromSystemClock() {
    //https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
    //https://developer.apple.com/documentation/kernel/1462446-mach_absolute_time
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    struct timespec ts;
    
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    
    uint64_t nanoseconds = ((uint64_t)ts.tv_sec)*NANOSECONDS_PER_SECONDS + (uint64_t)ts.tv_nsec;
    return nanoseconds;
  }

  uint64_t GetNanosecondsCounterFromMachAbsTime() {
    //https://stackoverflow.com/questions/41509505/clock-gettime-on-macos
    //https://developer.apple.com/library/archive/qa/qa1398/_index.html
    //https://stackoverflow.com/questions/25027215/queryperformancecounter-but-on-osx

    mach_timebase_info_data_t tbi;
    mach_timebase_info(&tbi);

    // Get absolute time in unknow units of time
    uint64_t base_time = mach_absolute_time();

    // Convert to nanoseconds
    uint64_t nanoseconds = (base_time * (uint64_t)tbi.numer) / (uint64_t)tbi.denom;
    return nanoseconds;
  }
  
#endif







  double GetMicrosecondsTimer() {
#ifdef _WIN32
    //For Windows 8 and up, the function GetSystemTimePreciseAsFileTime() 
    //should be used instead of QueryPerformanceCounter() as it have ~1.9 microseconds
    //accuracy and works on single and multiple core processors without having
    //to lock the thread on the same core.
    if (GetSystemTimePreciseAsFileTime_) {
      double seconds = GetSecondsFromSystemTime();
      return seconds;
    }

    //Fallback to using QueryPerformanceCounter() but the user must be aware that
    //if the current thread jumps to another core, the calclated elapsed time
    //will be incorrect or can even be backward. The code which is calculating the
    //performance/elapsed time should lock the thread to a single core.
    return GetSecondsFromPerformanceTimer();

#elif defined(__linux__)
    double seconds = GetMicrosecondsTimerFromMonotonicTimer();
    return seconds;
#elif defined(__APPLE__)
    double seconds = GetMicrosecondsTimerFromMachAbsTime();
    return seconds;
#else
    return -1.0;
#endif
  }

  double GetMillisecondsTimer() {
#ifdef _WIN32
    InitMillisecondsInterruptTimer();
    return GetSecondsFromTimeGetTime();
#elif defined(__linux__)
    double seconds = GetMicrosecondsTimerFromMonotonicCoarse();
    return seconds;
#elif defined(__APPLE__)
    double seconds = GetMillisecondsTimerFromMachAbsTime();
    return seconds;
#else
    return -1.0;
#endif
  }

  ///<summary>
  ///Returns the elasped time in milliseconds since an arbitrary starting point.
  ///</summary>
  uint64_t GetMillisecondsCounterU64() {
#ifdef _WIN32
    //There are 4 timer apis on Win32:
    // - GetNanosecondsCounterFromGetTickCount()      which has 15.6ms accuracy.
    // - GetNanosecondsCounterFromTimeGetTime()       which has 15.6ms accuracy by default. Can get as low as 1ms accuracy if after calling InitMillisecondsInterruptTimer().
    // - GetNanosecondsCounterFromSystemTime()        which has ~1ms accuracy on Windows 7 (and before), <1us accuracy on Windows 8 and up.
    // - GetNanosecondsCounterFromPerformanceTimer()  which has <1us accuracy, affected by multicore processors. Must always be called from the same thread.
    //
    //GetNanosecondsCounterFromSystemTime() has worst case milliseconds resolution on Windows 7.
    //From Windows 8 and up, its has <1ms resolution.
    uint64_t ns = GetNanosecondsCounterFromSystemTime();
    uint64_t ms = ns/1000000;
    return ms;
#elif defined(__linux__)
    uint64_t ns = GetNanosecondsCounterFromMonotonicCoarse();
    uint64_t ms = ns/1000000;
    return ms;
#elif defined(__APPLE__)
    uint64_t ns = GetNanosecondsCounterFromMachAbsTime();
    uint64_t ms = ns/1000000;
    return ms;
#else
    return 0;
#endif
  }

  ///<summary>
  ///Returns the elasped time in milliseconds since an arbitrary starting point.
  ///Note: the implementation is falling back on nanoseconds accuracy for best results.
  ///</summary>
  uint64_t GetMicrosecondsCounterU64() {
    uint64_t ns = GetNanosecondsCounterU64();
    uint64_t us = ns/1000;
    return us;
  }

  uint64_t GetNanosecondsCounterU64() {
#ifdef _WIN32
    //There are 4 timer apis on Win32:
    // - GetNanosecondsCounterFromGetTickCount()      which has 15.6ms accuracy.
    // - GetNanosecondsCounterFromTimeGetTime()       which has 15.6ms accuracy by default. Can get as low as 1ms accuracy if after calling InitMillisecondsInterruptTimer().
    // - GetNanosecondsCounterFromSystemTime()        which has ~1ms accuracy on Windows 7 (and before), <1us accuracy on Windows 8 and up.
    // - GetNanosecondsCounterFromPerformanceTimer()  which has <1us accuracy, affected by multicore processors. Must always be called from the same thread.
    //
    //For Windows 8 and up, the function GetSystemTimePreciseAsFileTime() 
    //should be used instead of QueryPerformanceCounter() as it have ~1.9 microseconds
    //accuracy and works on single and multiple core processors without having
    //to lock the thread on the same core.
    if (GetSystemTimePreciseAsFileTime_) {
      uint64_t nanoseconds = GetNanosecondsCounterFromSystemTime();
      return nanoseconds;
    }

    //Fallback to using QueryPerformanceCounter() but the user must be aware that
    //if the current thread jumps to another core, the calclated elapsed time
    //will be incorrect or can even be backward. The code which is calculating the
    //performance/elapsed time should lock the thread to a single core.
    uint64_t nanoseconds = GetNanosecondsCounterFromPerformanceTimer();
    return nanoseconds;

#elif defined(__linux__)
    uint64_t ns = GetNanosecondsCounterFromMonotonicTimer();
    return ns;
#elif defined(__APPLE__)
    uint64_t ns = GetNanosecondsCounterFromMachAbsTime();
    return ns;
#else
    return 0;
#endif
  }

  DateTime ToDateTime(const std::tm & time_info) {
    DateTime dt;

    dt.year  = time_info.tm_year + 1900;
    dt.month = time_info.tm_mon + 1;
    dt.day   = time_info.tm_mday;
    dt.hour  = time_info.tm_hour;
    dt.min   = time_info.tm_min;
    dt.sec   = time_info.tm_sec;
    dt.wday  = time_info.tm_wday;
    dt.yday  = time_info.tm_yday;
    dt.isdst = (time_info.tm_isdst != 0);

    return dt;
  }

  std::tm ToTimeInfo(const DateTime & date_time) {
    std::tm time_info;

    time_info.tm_year  = date_time.year - 1900;
    time_info.tm_mon   = date_time.month - 1;
    time_info.tm_mday  = date_time.day;
    time_info.tm_hour  = date_time.hour;
    time_info.tm_min   = date_time.min;
    time_info.tm_sec   = date_time.sec;
    time_info.tm_wday  = date_time.wday;
    time_info.tm_yday  = date_time.yday;
    time_info.tm_isdst = (date_time.isdst ? 1 : 0);

    return time_info;
  }

  void WaitNextSecond() {
    std::tm base_time = GetLocalTime();
    while (GetLocalTime().tm_sec == base_time.tm_sec) {
      //loop
    }
  }

  std::tm GetLocalTime() {
    time_t raw_time;
    std::time(&raw_time);

    std::tm time_info = *localtime(&raw_time);
    return time_info;
  }

  std::tm GetUtcTime() {
    time_t raw_time;
    std::time(&raw_time);

    std::tm time_info = *gmtime(&raw_time);
    return time_info;
  }

  int Millisleep(uint32_t milliseconds) {
    //code from https://stackoverflow.com/a/14818830 and https://stackoverflow.com/a/28827188
#if defined(_WIN32)
    SetLastError(0);
    Sleep(milliseconds);
    return GetLastError() ? -1 : 0;
#elif _POSIX_C_SOURCE >= 199309L
    /* prefer to use nanosleep() */
    const struct timespec ts = {
      (__time_t)milliseconds / 1000, /* seconds */
      ((__syscall_slong_t)milliseconds % 1000) * 1000 * 1000 /* nano seconds */
    };
    return nanosleep(&ts, NULL);
#elif __APPLE__
    return usleep(1000 * milliseconds);
#elif _BSD_SOURCE || (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
    /* else fallback to obsolte usleep() */
    return usleep(1000 * milliseconds);
#else
# error ("No millisecond sleep available for this platform!")
    return -1;
#endif
  }

  int GetYearFromCompilationDate(const char * compilation_date) {
    static const int DEFAULT_YEAR = 2016;
    if (compilation_date == NULL)
      return DEFAULT_YEAR;
    std::string compilation_date_str = __DATE__;
    size_t last_space_index = compilation_date_str.find_last_of(" ");
    if (last_space_index == std::string::npos)
      return DEFAULT_YEAR;
    const char * year_str = &compilation_date_str[last_space_index + 1];
    int year = atoi(year_str);
    return year;
  }

} //namespace timing
} //namespace ra
