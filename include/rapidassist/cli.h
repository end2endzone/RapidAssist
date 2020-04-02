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

#ifndef RA_CLI_H
#define RA_CLI_H

#include <string>

#include "rapidassist/config.h"

namespace ra { namespace cli {

  /// <summary>
  /// Parses an argument from the command line parameters (argc, argv)
  /// The argument must be in the form of --name=value or --name
  /// </summary>
  /// <param name="name">Name of the argument. Case sensitive</param>
  /// <param name="value">The value of the argument in --name=value</param>
  /// <param name="argc">The number of element in argv</param>
  /// <param name="argv">The array of const char * representing each argument within the command line.</param>
  /// <returns>True when an argument named [name] is found. False otherwise.</returns>
  bool ParseArgument(const std::string & name, std::string & value, int argc, char **argv);

  /// <summary>
  /// Parses an argument from the command line parameters (argc, argv)
  /// The argument must be in the form of --name=value or --name
  /// </summary>
  /// <param name="name">Name of the argument. Case sensitive</param>
  /// <param name="value">The value of the argument in --name=value</param>
  /// <param name="argc">The number of element in argv</param>
  /// <param name="argv">The array of const char * representing each argument within the command line.</param>
  /// <returns>True when an argument named [name] is found. False otherwise.</returns>
  bool ParseArgument(const std::string & name, int & value, int argc, char **argv);

  /// <summary>
  /// Parses an argument from the command line parameters (argc, argv)
  /// The argument must be in the form of --name=value or --name
  /// </summary>
  /// <param name="name">Name of the argument. Case sensitive</param>
  /// <param name="value">The value of the argument in --name=value</param>
  /// <param name="argc">The number of element in argv</param>
  /// <param name="argv">The array of const char * representing each argument within the command line.</param>
  /// <returns>True when an argument named [name] is found. False otherwise.</returns>
  bool ParseArgument(const std::string & name, size_t & value, int argc, char **argv);

} //namespace environment
} //namespace ra

#endif //RA_CLI_H
