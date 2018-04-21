#ifndef NATIVEFUNC_H
#define NATIVEFUNC_H

#include <stdint.h>
#include <ctime> //for struct tm

namespace ra
{
  namespace nativefunc
  {

    ///<summary>
    ///Wait for the clock to jump to the next second. Synchronize the time clock to a new second.
    ///</summary>
    void waitNextSecond();

    ///<summary>
    ///Returns the local system time.
    ///</summary>
    ///<return>Returns the local system time.<return>
    std::tm getLocalSystemTime();

    ///<summary>
    ///Sleep the current thread for the given amount of milliseconds
    ///</summary>
    ///<param name="iTime">The time in milliseconds where the thread must sleep.</param>
    ///<return>On successfully sleeping for the requested interval, function returns 0.
    ///        If the call is interrupted or encounters an error, then it returns -1<return>
    int millisleep(uint32_t milliseconds);

  }; //nativefunc
}; //namespace ra

#endif //NATIVEFUNC_H
