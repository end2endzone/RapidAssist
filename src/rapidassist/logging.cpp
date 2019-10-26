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

#include "rapidassist/logging.h"

#include <sstream>
#include <stdarg.h> //for functions with "..." arguments
#include <cstdlib> //for printf()
#include <cstdio> //for vsnprintf()

namespace ra
{
  namespace logging
  {
    //global flag to silence the logging output
    bool quiet_mode = false;

    void setQuietMode(bool iQuiet) {
      quiet_mode = iQuiet;
    }

    bool isQuietModeEnabled() {
      return quiet_mode;
    }

    void log(LoggerLevel iLevel, const char * iFormat, ...) {
      if (iFormat == NULL)
        return;

      std::string logstring;

      //convert arguments to a single string
      va_list args;
      va_start(args, iFormat);
      static const int BUFFER_SIZE = 10240;
      char buffer[BUFFER_SIZE];
      buffer[0] = '\0';
      vsnprintf(buffer, BUFFER_SIZE, iFormat, args);
      logstring = buffer;
      va_end(args);

      //print the single string to the console
      if (iLevel == LOG_INFO && quiet_mode)
        return; //silence the output

      if (iLevel == LOG_ERROR) {
        printf("Error: %s\n", logstring.c_str());
      }
      else if (iLevel == LOG_WARNING) {
        printf("Warning: %s\n", logstring.c_str());
      }
      else {
        printf("%s\n", logstring.c_str());
      }
    }

  } //namespace environment
} //namespace ra
