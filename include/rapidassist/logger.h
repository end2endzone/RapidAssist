#ifndef RA_LOGGER_H
#define RA_LOGGER_H

#include <string>

namespace ra
{
  namespace logger
  {

    ///<summary>
    ///Different logger levels
    ///</summary>
    enum LOGGER_LEVEL
    {
      LOG_INFO,
      LOG_WARNING,
      LOG_ERROR,
    };

    ///<summary>
    ///Sets the quiet mode enabled or disabled.
    ///Silences all log of level LOG_INFO 
    ///</summary>
    ///<param name="iQuiet">The new value of the quiet mode.</param>
    void setQuietMode(bool iQuiet);

    ///<summary>
    ///Returns true if the quiet mode is enabled.
    ///</summary>
    ///<return>Returns true if the quiet mode is enabled.<return>
    bool isQuietModeEnabled();

    ///<summary>
    ///Prints the given arguments to the console depending on the specified logging level.
    ///</summary>
    ///<param name="iLevel">The level of the given arguments</param>
    ///<param name="iFormat">The format of the given argument. Same as printf's format.</param>
    void log(LOGGER_LEVEL iLevel, const char * iFormat, ...);


  } //namespace environment
} //namespace ra

#endif //RA_LOGGER_H