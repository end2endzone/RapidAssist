#ifndef RA_ENVIRONMENT_H
#define RA_ENVIRONMENT_H

#include <string>

namespace ra
{
  namespace environment
  {

    ///<summary>
    ///Returns value of an environment variable.
    ///</summary>
    ///<param name="iName">Name of the variable</param>
    ///<return>Returns the value of the given environment variable.<return>
    std::string getEnvironmentVariable(const char * iName);

    ///<summary>
    ///Returns if the current process is 32 bit.
    ///</summary>
    ///<return>Returns true if the current process is 32 bit. Returns false otherwise.<return>
    bool isProcess32Bit();

    ///<summary>
    ///Returns if the current process is 64 bit.
    ///</summary>
    ///<return>Returns true if the current process is 64 bit. Returns false otherwise.<return>
    bool isProcess64Bit();

    ///<summary>
    ///Returns if the current process is in Debug mode.
    ///</summary>
    ///<return>Returns true if the current process is in Debug mode. Returns false otherwise.<return>
    bool isConfigurationDebug();

    ///<summary>
    ///Returns if the current process is in Release mode.
    ///</summary>
    ///<return>Returns true if the current process is in Release mode. Returns false otherwise.<return>
    bool isConfigurationRelease();

  } //namespace environment
} //namespace ra

#endif //RA_ENVIRONMENT_H
