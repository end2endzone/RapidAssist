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

#include "rapidassist/cli.h"
#include <cstdlib> //for atoi()
#include <cstdio> //for sscanf()

namespace ra { namespace cli {

  bool ParseArgument(const std::string & name, std::string & value, int argc, char **argv) {
    value = "";

    //look for --name=value pattern
    std::string pattern;
    pattern.append("--");
    pattern.append(name);
    pattern.append("=");
    for (int i = 0; i < argc; i++) {
      std::string arg = argv[i];
      if (arg.length() > pattern.length() && arg.substr(0, pattern.length()) == pattern) {
        value = arg.substr(pattern.length(), arg.length() - pattern.length());
        return true;
      }
    }

    //look for exactly --name pattern
    pattern = "";
    pattern.append("--");
    pattern.append(name);
    for (int i = 0; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == pattern) {
        value = "";
        return true;
      }
    }

    return false;
  }

  bool ParseArgument(const std::string & name, int & value, int argc, char **argv) {
    std::string tmp;
    bool found = ParseArgument(name, tmp, argc, argv);
    if (found) {
      value = atoi(tmp.c_str());
      return true;
    }
    return false;
  }

  bool ParseArgument(const std::string & name, size_t & value, int argc, char **argv) {
    std::string tmp;
    bool found = ParseArgument(name, tmp, argc, argv);
    if (found) {
#ifdef _WIN32
      static const char * format = "%Iu";
#elif defined(__linux__) || defined(__APPLE__)
      static const char * format = "%zu";
#endif
      if (1 == sscanf(tmp.c_str(), format, &value)) {
        return true;
      }
      else {
        return false;
      }
    }
    return false;
  }

} //namespace environment
} //namespace ra
