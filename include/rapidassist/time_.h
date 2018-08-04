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

#ifndef RA_TIME_H
#define RA_TIME_H

#include <stdint.h>
#include <ctime> //for struct tm

namespace ra
{
  namespace time
  {

    /// <summary>
    /// DATETIME stucture with human readable values
    /// </summary>
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

    /// <summary>
    /// Converts a std::tm structure to a DATETIME structure.
    /// </summary>
    /// <param name="timeinfo">The given std::tm value.</param>
    /// <returns>Returns a DATETIME structure matching the given std::tm info.</returns>
    DATETIME toDateTime(const std::tm & timeinfo);

    /// <summary>
    /// Converts a DATETIME structure to a std::tm structure.
    /// </summary>
    /// <param name="iDateTime">The given DATETIME value.</param>
    /// <returns>Returns a std::tm structure matching the given DATETIME info.</returns>
    std::tm toTimeInfo(const DATETIME & iDateTime);

    /// <summary>
    /// Wait for the clock to jump to the next second. Synchronize the time clock to a new second.
    /// </summary>
    void waitNextSecond();

    /// <summary>
    /// Returns the system local time.
    /// </summary>
    /// <returns>Returns the system local time.</returns>
    std::tm getLocalTime();

    /// <summary>
    /// Returns the system utc time.
    /// </summary>
    /// <returns>Returns the system utc time.</returns>
    std::tm getUtcTime();

    /// <summary>
    /// Sleep the current thread for the given amount of milliseconds
    /// </summary>
    /// <param name="milliseconds">The time in milliseconds where the thread must sleep.</param>
    /// <returns>On successfully sleeping for the requested interval, function returns 0.
    ///         If the call is interrupted or encounters an error, then it returns -1</returns>
    int millisleep(uint32_t milliseconds);

    /// <summary>
    /// Returns the year when the code was build.
    /// Usefull for copyright messages.
    /// </summary>
    /// <returns>Returns the year when the function was compiled.</returns>
    int getCopyrightYear();

    /// <summary>
    /// Portable function to calculate the elapsed time with microseconds resolution.
    /// </summary>
    /// <remarks>The function query the internal high resolution clock/timer to calculate the elapsed time.</remarks>
    /// <returns>Returns the elapsed time in seconds since an arbitrary starting point with microseconds resolution. Returns -1.0 on error.</returns>
    double getMicrosecondsTimer();

    /// <summary>
    /// Portable function to calculate the elapsed time with milliseconds resolution.
    /// </summary>
    /// <remarks>The function uses the internal clock to calculate the elapsed time.</remarks>
    /// <returns>Returns the elapsed time in seconds since an arbitrary starting point with milliseconds resolution. Returns -1.0 on error.</returns>
    double getMillisecondsTimer();

  }; //namespace time
} //namespace ra

#endif //RA_TIME_H
