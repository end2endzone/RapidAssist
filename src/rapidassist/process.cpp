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

#include "rapidassist/process.h"
#include "rapidassist/filesystem.h"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h> // for GetModuleHandleEx()
#endif

namespace ra
{
  namespace process
  {

    std::string getCurrentProcessPath()
    {
      std::string path;
#ifdef _WIN32
      HMODULE hModule = GetModuleHandle(NULL);
      if (hModule == NULL)
      {
        int ret = GetLastError();
        return path; //failure
      }
      //get the path of this process
      char buffer[MAX_PATH] = {0};
      if (!GetModuleFileName(hModule, buffer, sizeof(buffer)))
      {
        int ret = GetLastError();
        return path; //failure
      }
      path = buffer;
#elif __linux__
#endif
      //not supported
      return path;
    }

    std::string getCurrentProcessDir()
    {
      std::string dir;
      std::string execPath = getCurrentProcessPath();
      if (execPath.empty())
        return dir; //failure
      dir = ra::filesystem::getParentPath(execPath);
      return dir;
    }

  } //namespace process
} //namespace ra
