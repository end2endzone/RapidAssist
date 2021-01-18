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

#include "rapidassist/environment_utf8.h"
#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"
#include <cstdlib>  //for getenv()
#include <cstring>  //for strlen()
#include <stdlib.h> //for setenv(), unsetenv()
#include <stdio.h>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
  #endif /* WIN32_LEAN_AND_MEAN */
  #include <Windows.h>
  #include "rapidassist/undef_windows_macros.h"
#endif

namespace ra { namespace environment {

#ifdef _WIN32 // UTF-8

  std::string GetEnvironmentVariableUtf8(const char * name) {
    if (name == NULL)
      return std::string();

    std::wstring nameW = ra::unicode::Utf8ToUnicode(name);

    const wchar_t * value = _wgetenv(nameW.c_str());
    if (value == NULL)
      return std::string();

    std::string value_utf8 = ra::unicode::UnicodeToUtf8(value);
    return value_utf8;
  }

  bool SetEnvironmentVariableUtf8(const char * name, const char * value) {
    //validate invalid inputs
    if (name == NULL || strlen(name) == 0) {
      return false;
    }

    std::wstring nameW  = ra::unicode::Utf8ToUnicode(name);

    std::wstring commandW;
    commandW.append(nameW);
    commandW.append(L"=");
    if (value) {
      std::wstring valueW = ra::unicode::Utf8ToUnicode(value);
      commandW.append(valueW);
    }
    int result = _wputenv(commandW.c_str());

    bool success = (result == 0);
    return success;
  }

  ra::strings::StringVector GetEnvironmentVariablesUtf8() {
    ra::strings::StringVector vars;

#ifdef _WIN32
    // Get a pointer to the environment block.
    LPWCH lpvEnv = GetEnvironmentStringsW();

    // If the returned pointer is NULL, exit.
    if (lpvEnv == NULL)
      return vars;
 
    // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
    LPWSTR lpvTmp = (LPWSTR)lpvEnv;
    while (*lpvTmp)
    {
      std::wstring definition = lpvTmp;

      // Skip "current directory" and drive environment variables:
      //  "=::=::\"
      //  "=C:=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE"
      //  "=G:=G:\Temp\temp3"
      if (!definition.empty() && definition[0] != '=')
      {
        size_t offset = definition.find('=');
        if (offset != std::string::npos) {
          std::wstring nameW = definition.substr(0, offset);
          //std::wstring valueW = definition.substr(offset + 1);

          std::string name_utf8  = ra::unicode::UnicodeToUtf8(nameW);
          //std::string value_utf8 = ra::unicode::UnicodeToUtf8(valueW);

          vars.push_back(name_utf8);
        }
      }

      //next definition
      lpvTmp += lstrlenW(lpvTmp) + 1;
    }
    FreeEnvironmentStringsW(lpvEnv);
#else
    wchar_t *s = *_wenviron;

    int i = 0;
    s = *(_wenviron + i);

    while (s) {
      std::wstring definition = s;
      size_t offset = definition.find('=');
      if (offset != std::string::npos) {
        std::wstring nameW = definition.substr(0, offset);
        std::wstring valueW = definition.substr(offset + 1);
        int a = 0;

        std::string name_utf8  = ra::unicode::UnicodeToUtf8(nameW);
        std::string value_utf8 = ra::unicode::UnicodeToUtf8(valueW);

        vars.push_back(name_utf8);
      }

      //next var
      i++;
      s = *(_wenviron + i);
    }
#endif
    return vars;
  }

  std::string ExpandUtf8(const std::string & value) {
    std::string output = value;

    ra::strings::StringVector variables = GetEnvironmentVariablesUtf8();
    for (size_t i = 0; i < variables.size(); i++) {
      const std::string & name = variables[i];

      std::string pattern = std::string("%") + name + std::string("%");
      std::string value = ra::environment::GetEnvironmentVariableUtf8(name.c_str());

      //process with search and replace
      ra::strings::Replace(output, pattern, value);

      //On Windows, the expansion is not case sensitive.
      //also look for case insensitive replacement
      std::string pattern_uppercase = ra::strings::Uppercase(pattern);
      std::string output_uppercase = ra::strings::Uppercase(output);
      size_t pattern_pos = output_uppercase.find(pattern_uppercase);
      while (pattern_pos != std::string::npos) {
        //extract the pattern from the value.
        //ie: the value contains %systemdrive% instead of the official %SystemDrive%
        std::string pattern2 = output.substr(pattern_pos, pattern.size());

        //process with search and replace using the unofficial pattern
        ra::strings::Replace(output, pattern2, value);

        //search again for next pattern
        output_uppercase = ra::strings::Uppercase(output);
        pattern_pos = output_uppercase.find(pattern_uppercase);
      }
    }

    return output;
  }

#endif // UTF-8

} //namespace environment
} //namespace ra
