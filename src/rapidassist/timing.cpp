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

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h> // for Sleep()
#undef min
#undef max
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep()
#else
#include <unistd.h> // for usleep()
#endif

#ifdef WIN32
#include <Mmsystem.h> //for timeGetTime()
#pragma comment(lib, "winmm.lib") //for timeGetTime()
#else
#include <sys/time.h> //for clock_gettime()
#endif

#include <time.h> //for time() and localtime()
#include <string>

namespace ra { namespace timing {

#ifdef __linux__
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
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  double GetPerformanceTimerWin32() {
    //Warning for processes running a multicore processors...
    //While QueryPerformanceCounter and QueryPerformanceFrequency typically adjust
    //for multiple processors, bugs in the BIOS or drivers may result in these routines
    //returning different values as the thread moves from one processor to another.
    //So, it's best to keep the thread on a single processor.
    //See the following for details:
    //  https://msdn.microsoft.com/en-us/library/windows/desktop/ee417693(v=vs.85).aspx
    //  https://stackoverflow.com/questions/44020619/queryperformancecounter-on-multi-core-processor-under-windows-10-behaves-erratic
    //  https://msdn.microsoft.com/en-us/library/windows/desktop/ms686247(v=vs.85).aspx (SetThreadAffinityMask function)

    static LARGE_INTEGER frequency = { 0 };
    //if first pass
    if (frequency.QuadPart == 0)
      QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    double seconds = counter.QuadPart / double(frequency.QuadPart);
    return seconds;
  }

  ///<summary>
  ///Initialize the multimedia timer to have a 1 millisecond resolution.
  ///By default, the resolution is 15.6ms (64Hz).
  ///The function must be called once per process execution.
  ///</summary>
  void InitMillisecondsInterruptTimer() {
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
  double GetTickCountTimer() //fast constant 15ms timer
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
  double GetMillisecondsTimerWin32() {
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
  ///  - Have ~2us accuracy on Windows 8 and up.
  ///  - Uses the multimedia timer. See also InitMillisecondsInterruptTimer().
  ///</remarks>
  ///<returns>Returns the elapsed time in seconds since an arbitrary starting point.</returns>
  double GetSystemTimeTimerWin32() {
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
#endif







  double GetMicrosecondsTimer() {
#ifdef WIN32
    //For Windows 8 and up, the function GetSystemTimePreciseAsFileTime() 
    //should be used instead of QueryPerformanceCounter() as it have ~1.9 microseconds
    //accuracy and works on single and multiple core processors without having
    //to lock the thread on the same core.
    if (GetSystemTimePreciseAsFileTime_) {
      double seconds = GetSystemTimeTimerWin32();
      return seconds;
    }

    //Fallback to using QueryPerformanceCounter() but the user must be aware that
    //if the current thread jumps to another core, the calclated elapsed time
    //will be incorrect or can even be backward. The code which is calculating the
    //performance/elapsed time should lock the thread to a single core.
    return GetPerformanceTimerWin32();

#elif __linux__
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
#else
    return -1.0;
#endif
  }

  double GetMillisecondsTimer() {
#ifdef WIN32
    InitMillisecondsInterruptTimer();
    return GetMillisecondsTimerWin32();
#elif __linux__
    struct timespec now;
    clockid_t clock_id = 0;

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
#else
    return -1.0;
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

  std::tm ToTimeInfo(const DateTime & iDateTime) {
    std::tm time_info;

    time_info.tm_year  = iDateTime.year - 1900;
    time_info.tm_mon   = iDateTime.month - 1;
    time_info.tm_mday  = iDateTime.day;
    time_info.tm_hour  = iDateTime.hour;
    time_info.tm_min   = iDateTime.min;
    time_info.tm_sec   = iDateTime.sec;
    time_info.tm_wday  = iDateTime.wday;
    time_info.tm_yday  = iDateTime.yday;
    time_info.tm_isdst = (iDateTime.isdst ? 1 : 0);

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
#if defined(WIN32)
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
#elif _BSD_SOURCE || \
(_XOPEN_SOURCE >= 500 || \
_XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && \
!(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)

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
    const char * yearStr = &compilation_date_str[last_space_index + 1];
    int year = atoi(yearStr);
    return year;
  }

} //namespace timing
} //namespace ra
