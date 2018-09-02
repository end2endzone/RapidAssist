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

#include "rapidassist/console.h"
#include "rapidassist/time_.h"
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#endif

namespace ra
{
  namespace console
  {
    struct CursorCoordinate
    {
      int x;
      int y;
    };

    std::vector<CursorCoordinate> gCursorPositionStack;

    void getCursorPos(int & x, int & y)
    {
#ifdef _WIN32
		  CONSOLE_SCREEN_BUFFER_INFO info;
		  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		  x = info.dwCursorPosition.X;
		  y = info.dwCursorPosition.Y;
#elif __linux__
      //not implemented yet
      x = 0;
      y = 0;
#endif
    }

    void setCursorPos(const int & x, const int & y)
    {
#ifdef _WIN32
		  COORD coord;
		  coord.X = x;
		  coord.Y = y;
		  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#elif __linux__
      //not implemented yet
#endif
    }

    void clearScreen()
    {
		  //system("cls");

#ifdef _WIN32
      COORD coordScreen = { 0, 0 };
      DWORD cCharsWritten;
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      DWORD dwConSize;
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      GetConsoleScreenBufferInfo(hConsole, &csbi);
      dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
      FillConsoleOutputCharacter(hConsole, TEXT(' '),
                                dwConSize,
                                coordScreen,
                                &cCharsWritten);
      GetConsoleScreenBufferInfo(hConsole, &csbi);
      FillConsoleOutputAttribute(hConsole,
                                csbi.wAttributes,
                                dwConSize,
                                coordScreen,
                                &cCharsWritten);
      SetConsoleCursorPosition(hConsole, coordScreen);
#elif __linux__
      //not implemented yet
#endif
    }

    void pushCursorPos()
    {
      CursorCoordinate coord;
      getCursorPos(coord.x, coord.y);

      gCursorPositionStack.push_back(coord);
    }

    void popCursorPos()
    {
      if (!gCursorPositionStack.empty())
      {
        size_t offset = gCursorPositionStack.size() - 1;
        const CursorCoordinate & last = gCursorPositionStack[offset];
        setCursorPos(last.x, last.y);

        //remove the last entry
        gCursorPositionStack.erase(gCursorPositionStack.begin() + offset);
      }
    }

    char getAnimationSprite(double iRefreshRate)
    {
      static const char gAnimationSprites[] = {'-', '\\', '|', '/'};
      static const int gNumAnimationSprites = sizeof(gAnimationSprites)/sizeof(gAnimationSprites[0]);
      double seconds = ra::time::getMillisecondsTimer(); //already seconds
      int spriteIndex = (int)(seconds/iRefreshRate);
      spriteIndex = spriteIndex % gNumAnimationSprites;
      char sprite = gAnimationSprites[spriteIndex];
      return sprite;
    }
 
    void printAnimationCursor()
    {
      pushCursorPos();
      printf("%c", getAnimationSprite(0.15));
      popCursorPos();
    }

  } //namespace console
} //namespace ra
