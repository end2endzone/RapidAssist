#ifndef RA_TIME_H
#define RA_TIME_H

#include <stdint.h>
#include <ctime> //for struct tm

namespace ra
{
  namespace time
  {

    ///<summary>
    ///DATETIME stucture with human readable values
    ///</summary>
    struct DATETIME
    {
      uint16_t year;  // year - [0, 65000]
      uint16_t month; // month of the year - [1,12]
      uint16_t day;   // day of the month - [1,31]
      uint16_t hour;  // hours since midnight - [0,23]
      uint16_t min;   // minutes after the hour - [0,59]
      uint16_t sec;   // seconds after the minute - [0,59]
      uint16_t wday;  // week day - [0,6]
      uint16_t yday;  // year day - [0,365]
      bool isdst;     // daylight savings time flag
    };

    ///<summary>
    ///Converts a std::tm structure to a DATETIME structure.
    ///</summary>
    ///<param name="timeinfo">The given std::tm value.</param>
    ///<return>Returns a DATETIME structure matching the given std::tm info.<return>
    DATETIME toDateTime(const std::tm & timeinfo);

    ///<summary>
    ///Converts a DATETIME structure to a std::tm structure.
    ///</summary>
    ///<param name="iDateTime">The given DATETIME value.</param>
    ///<return>Returns a std::tm structure matching the given DATETIME info.<return>
    std::tm toTimeInfo(const DATETIME & iDateTime);

    ///<summary>
    ///Wait for the clock to jump to the next second. Synchronize the time clock to a new second.
    ///</summary>
    void waitNextSecond();

    ///<summary>
    ///Returns the system local time.
    ///</summary>
    ///<return>Returns the system local time.<return>
    std::tm getLocalTime();

    ///<summary>
    ///Returns the system utc time.
    ///</summary>
    ///<return>Returns the system utc time.<return>
    std::tm getUtcTime();

    ///<summary>
    ///Sleep the current thread for the given amount of milliseconds
    ///</summary>
    ///<param name="iTime">The time in milliseconds where the thread must sleep.</param>
    ///<return>On successfully sleeping for the requested interval, function returns 0.
    ///        If the call is interrupted or encounters an error, then it returns -1<return>
    int millisleep(uint32_t milliseconds);

    ///<summary>
    ///Returns the year when the code was build.
    ///Usefull for copyright messages.
    ///</summary>
    ///<return>Returns the year when the function was compiled.<return>
    int getCopyrightYear();

    ///<summary>
    ///Query the internal high resolution clock/timer to calculate the elapsed time since an undefined starting point.
    ///</summary>
    ///<returns>Returns the elapsed time in seconds since an undefined starting point.</returns>
    double getMicrosecondsTimer();

  }; //namespace time
} //namespace ra

#endif //RA_TIME_H
