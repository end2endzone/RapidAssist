#include "time_.h"

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

#include <time.h> //for time() and localtime()
#include <string>

namespace ra
{

  namespace time
  {
    DATETIME toDateTime(const std::tm & timeinfo)
    {
      DATETIME dt;

      dt.year  = timeinfo.tm_year + 1900;
      dt.month = timeinfo.tm_mon + 1;
      dt.day   = timeinfo.tm_mday;
      dt.hour  = timeinfo.tm_hour;
      dt.min   = timeinfo.tm_min;
      dt.sec   = timeinfo.tm_sec;
      dt.wday  = timeinfo.tm_wday;
      dt.yday  = timeinfo.tm_yday;
      dt.isdst = (timeinfo.tm_isdst != 0);

      return dt;
    }

    std::tm toTimeInfo(const DATETIME & iDateTime)
    {
      std::tm timeinfo;

      timeinfo.tm_year  = iDateTime.year - 1900;
      timeinfo.tm_mon   = iDateTime.month - 1;
      timeinfo.tm_mday  = iDateTime.day;
      timeinfo.tm_hour  = iDateTime.hour;
      timeinfo.tm_min   = iDateTime.min;
      timeinfo.tm_sec   = iDateTime.sec;
      timeinfo.tm_wday  = iDateTime.wday;
      timeinfo.tm_yday  = iDateTime.yday;
      timeinfo.tm_isdst = (iDateTime.isdst ? 1 : 0);

      return timeinfo;
    }

    void waitNextSecond()
    {
      std::tm baseTime = getLocalTime();
      while(getLocalTime().tm_sec == baseTime.tm_sec)
      {
        //loop
      }
    }

    std::tm getLocalTime()
    {
      time_t rawtime;
      std::time(&rawtime);
      
      std::tm timeinfo = *localtime(&rawtime);
      return timeinfo;
    }

    std::tm getUtcTime()
    {
      time_t rawtime;
      std::time(&rawtime);
      
      std::tm timeinfo = *gmtime(&rawtime);
      return timeinfo;
    }

    int millisleep(uint32_t milliseconds)
    {
      //code from https://stackoverflow.com/a/14818830 and https://stackoverflow.com/a/28827188
#if defined(WIN32)
      SetLastError(0);
      Sleep(milliseconds);
      return GetLastError() ?-1 :0;
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

    int getCopyrightYear()
    {
      static const int DEFAULT_YEAR = 2016;
      std::string compilationDate = __DATE__;
      size_t lastSpace = compilationDate.find_last_of(" ");
      if (lastSpace == std::string::npos)
        return DEFAULT_YEAR;
      const char * yearStr = &compilationDate[lastSpace+1];
      int year = atoi(yearStr);
      return year;
    }


  }; //namespace time
} //namespace ra
