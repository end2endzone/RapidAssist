#ifndef RA_ENVIRONMENT_H
#define RA_ENVIRONMENT_H

#include <string>

namespace ra
{
  namespace environment
  {

    /// <summary>
    /// Returns value of an environment variable.
    /// </summary>
    /// <param name="iName">Name of the variable</param>
    /// <returns>Returns the value of the given environment variable.</returns>
    std::string getEnvironmentVariable(const char * iName);

    /// <summary>
    /// Returns if the current process is 32 bit.
    /// </summary>
    /// <returns>Returns true if the current process is 32 bit. Returns false otherwise.</returns>
    bool isProcess32Bit();

    /// <summary>
    /// Returns if the current process is 64 bit.
    /// </summary>
    /// <returns>Returns true if the current process is 64 bit. Returns false otherwise.</returns>
    bool isProcess64Bit();

    /// <summary>
    /// Returns if the current process is in Debug mode.
    /// </summary>
    /// <returns>Returns true if the current process is in Debug mode. Returns false otherwise.</returns>
    bool isConfigurationDebug();

    /// <summary>
    /// Returns if the current process is in Release mode.
    /// </summary>
    /// <returns>Returns true if the current process is in Release mode. Returns false otherwise.</returns>
    bool isConfigurationRelease();

    /// <summary>
    /// Returns the line separator of the current system.
    /// </summary>
    /// <returns>Returns the line separator for the current system.</returns>
    const char * getLineSeparator();

  } //namespace environment
} //namespace ra

#endif //RA_ENVIRONMENT_H
