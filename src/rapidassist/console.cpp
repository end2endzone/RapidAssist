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
#include "rapidassist/environment.h"
#include <vector>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif /* WIN32_LEAN_AND_MEAN */
#include <Windows.h>

#elif __linux__

#include <cstdio>       // fileno()
#include <unistd.h>     // isatty()
#include <sys/ioctl.h>  // ioctl()
#include <unistd.h>

//for getCursorPos()
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

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

    void getCursorPos(int & col, int & row)
    {
      col = 0;
      row = 0;

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      col = csbiInfo.dwCursorPosition.X;
      row = csbiInfo.dwCursorPosition.Y;
#elif __linux__
      //flush whatever was printed before
      fflush(stdout);

      struct termios torig, traw;
      tcgetattr(0, &torig);
      memcpy(&traw, &torig, sizeof(struct termios));
      cfmakeraw(&traw);
      traw.c_cc[VMIN] = 1;
      traw.c_cc[VTIME] = 0;
      tcsetattr(0, TCSANOW, &traw);

      static const int MAX_LOOP_COUNT = 100; //prevent infinite loops in the program
      int loopcount = 0;
      bool success = false;
      char buf[32];
      while(!success && loopcount < MAX_LOOP_COUNT)
      {
        ssize_t numWrite = write(0, "\033[6n", 4);
        fflush(stdout);

        ssize_t numRead = 0;
        if (numWrite == 4)
          numRead = read(0, buf, sizeof(buf)-1);

        //validate format \033[63;1R
        if(numWrite == 4 && numRead >= 6 && buf[0] == '\033' && buf[numRead - 1] == 'R')
        {
          buf[0] = '!'; //in case we want to print buf for debugging
          buf[numRead] = '\0';

          //try to parse the result
          const char * lastchar = &buf[numRead];
          const char * nextchar = &buf[1];

          if (nextchar[0] == '[')
            nextchar++; //next character
          else
            nextchar = lastchar; //fail parsing. jump to the end of the string

          //parse row
          row = 0;
          while (nextchar[0] >= '0' && nextchar[0] <= '9')
          {
            row = 10 * row + nextchar[0] - '0';
            nextchar++; //next character
          }

          if (nextchar[0] == ';')
            nextchar++; //next character
          else
            nextchar = lastchar; //fail parsing. jump to the end of the string

          //parse col
          col = 0;
          while (nextchar[0] >= '0' && nextchar[0] <= '9')
          {
            col = 10 * col + nextchar[0] - '0';
            nextchar++; //next character
          }

          if (nextchar[0] == 'R')
            success = true;
        }

        loopcount++;
      }

      tcsetattr(0, TCSANOW, &torig);

      if (success)
      {
        col--; //convert from ANSI 1-based to 0-based
      }
#endif
    }

    void setCursorPos(const int & col, const int & row)
    {
#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      COORD coord;
      coord.X = col;
      coord.Y = row;
      if (!SetConsoleCursorPosition(hStdout, coord))
      {
        printf("SetConsoleCursorPosition() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      printf("\033[%d;%dH", row, col);
#endif
    }

    void getDimension(int & width, int & height)
    {
      width = 0;
      height = 0;
#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      width = (int)csbiInfo.dwMaximumWindowSize.X;
      height = (int)csbiInfo.dwMaximumWindowSize.Y;
#elif __linux__
      struct winsize ws;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
      width = (int)ws.ws_col;
      height = (int)ws.ws_row;
#endif
    }

    void clearScreen()
    {
      //system("cls");

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      COORD coord = { 0 , 0 };
      DWORD dwNumberOfCharsWritten;
      DWORD dwConSize;
      dwConSize = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
      if (!FillConsoleOutputCharacter(hStdout, TEXT(' '), dwConSize, coord, &dwNumberOfCharsWritten))
      {
        printf("FillConsoleOutputCharacter() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!FillConsoleOutputAttribute(hStdout, csbiInfo.wAttributes, dwConSize, coord, &dwNumberOfCharsWritten))
      {
        printf("FillConsoleOutputAttribute() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!SetConsoleCursorPosition(hStdout, coord))
      {
        printf("SetConsoleCursorPosition() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      //not implemented yet
      printf("\033[2J");
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

    const char * getTextColorName(const TextColor & color)
    {
      switch(color)
      {
      case Black:
        return "Black";
        break;
      case White:
        return "White";
        break;
      case DarkBlue:
        return "DarkBlue";
        break;
      case DarkGreen:
        return "DarkGreen";
        break;
      case DarkCyan:
        return "DarkCyan";
        break;
      case DarkRed:
        return "DarkRed";
        break;
      case DarkMagenta:
        return "DarkMagenta";
        break;
      case DarkYellow:
        return "DarkYellow";
        break;
      case DarkGray:
        return "DarkGray";
        break;
      case Blue:
        return "Blue";
        break;
      case Green:
        return "Green";
        break;
      case Cyan:
        return "Cyan";
        break;
      case Red:
        return "Red";
        break;
      case Magenta:
        return "Magenta";
        break;
      case Yellow:
        return "Yellow";
        break;
      case Gray:
        return "Gray";
        break;
      default:
        return "";
      };
    }

    const char * ansi::FormatAttribute::toString(const ansi::FormatAttribute::Attr & attr)
    {
      switch(attr)
      {
      case ansi::FormatAttribute::Default    : return "Default"   ; break;
      case ansi::FormatAttribute::Bold       : return "Bold"      ; break;
      case ansi::FormatAttribute::Dim        : return "Dim"       ; break;
      case ansi::FormatAttribute::Underlined : return "Underlined"; break;
      case ansi::FormatAttribute::Blink      : return "Blink"     ; break;
      case ansi::FormatAttribute::Reverse    : return "Reverse"   ; break;
      case ansi::FormatAttribute::Hidden     : return "Hidden"    ; break;
      };
      return "Unknown";
    }

    const char * ansi::ForegroundColor::toString(const ansi::ForegroundColor::Color & color)
    {
      switch(color)
      {
      case ansi::ForegroundColor::Default      : return "Default"      ; break;
      case ansi::ForegroundColor::Black        : return "Black"        ; break;
      case ansi::ForegroundColor::Red          : return "Red"          ; break;
      case ansi::ForegroundColor::Green        : return "Green"        ; break;
      case ansi::ForegroundColor::Yellow       : return "Yellow"       ; break;
      case ansi::ForegroundColor::Blue         : return "Blue"         ; break;
      case ansi::ForegroundColor::Magenta      : return "Magenta"      ; break;
      case ansi::ForegroundColor::Cyan         : return "Cyan"         ; break;
      case ansi::ForegroundColor::LightGray    : return "LightGray"    ; break;
      case ansi::ForegroundColor::DarkGray     : return "DarkGray"     ; break;
      case ansi::ForegroundColor::LightRed     : return "LightRed"     ; break;
      case ansi::ForegroundColor::LightGreen   : return "LightGreen"   ; break;
      case ansi::ForegroundColor::LightYellow  : return "LightYellow"  ; break;
      case ansi::ForegroundColor::LightBlue    : return "LightBlue"    ; break;
      case ansi::ForegroundColor::LightMagenta : return "LightMagenta" ; break;
      case ansi::ForegroundColor::LightCyan    : return "LightCyan"    ; break;
      case ansi::ForegroundColor::White        : return "White"        ; break;
      };
      return "Unknown";
    }

    const char * ansi::BackgroundColor::toString(const ansi::BackgroundColor::Color & color)
    {
      switch(color)
      {
      case ansi::BackgroundColor::Default      : return "Default"      ; break;
      case ansi::BackgroundColor::Black        : return "Black"        ; break;
      case ansi::BackgroundColor::Red          : return "Red"          ; break;
      case ansi::BackgroundColor::Green        : return "Green"        ; break;
      case ansi::BackgroundColor::Yellow       : return "Yellow"       ; break;
      case ansi::BackgroundColor::Blue         : return "Blue"         ; break;
      case ansi::BackgroundColor::Magenta      : return "Magenta"      ; break;
      case ansi::BackgroundColor::Cyan         : return "Cyan"         ; break;
      case ansi::BackgroundColor::LightGray    : return "LightGray"    ; break;
      case ansi::BackgroundColor::DarkGray     : return "DarkGray"     ; break;
      case ansi::BackgroundColor::LightRed     : return "LightRed"     ; break;
      case ansi::BackgroundColor::LightGreen   : return "LightGreen"   ; break;
      case ansi::BackgroundColor::LightYellow  : return "LightYellow"  ; break;
      case ansi::BackgroundColor::LightBlue    : return "LightBlue"    ; break;
      case ansi::BackgroundColor::LightMagenta : return "LightMagenta" ; break;
      case ansi::BackgroundColor::LightCyan    : return "LightCyan"    ; break;
      case ansi::BackgroundColor::White        : return "White"        ; break;
      };
      return "Unknown";
    }

    void setTextColor(const TextColor & iForeground, const TextColor & iBackground)
    {
#ifdef _WIN32
      WORD foregroundAttribute = 0;
      switch(iForeground)
      {
      case Black:
        foregroundAttribute = 0;
        break;
      case White:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DarkBlue:
        foregroundAttribute = FOREGROUND_BLUE;
        break;
      case DarkGreen:
        foregroundAttribute = FOREGROUND_GREEN;
        break;
      case DarkCyan:
        foregroundAttribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DarkRed:
        foregroundAttribute = FOREGROUND_RED;
        break;
      case DarkMagenta:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case DarkYellow:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case DarkGray:
        foregroundAttribute = FOREGROUND_INTENSITY;
        break;
      case Blue:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
        break;
      case Green:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        break;
      case Cyan:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case Red:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
      case Magenta:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case Yellow:
        foregroundAttribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case Gray:
        foregroundAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      };

      WORD backgroundAttribute = 0;
      switch(iBackground)
      {
      case Black:
        backgroundAttribute = 0;
        break;
      case White:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DarkBlue:
        backgroundAttribute = BACKGROUND_BLUE;
        break;
      case DarkGreen:
        backgroundAttribute = BACKGROUND_GREEN;
        break;
      case DarkCyan:
        backgroundAttribute = BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DarkRed:
        backgroundAttribute = BACKGROUND_RED;
        break;
      case DarkMagenta:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case DarkYellow:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case DarkGray:
        backgroundAttribute = BACKGROUND_INTENSITY;
        break;
      case Blue:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_BLUE;
        break;
      case Green:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN;
        break;
      case Cyan:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case Red:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED;
        break;
      case Magenta:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case Yellow:
        backgroundAttribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case Gray:
        backgroundAttribute = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      };

      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }

      if (!SetConsoleTextAttribute(hStdout, foregroundAttribute | backgroundAttribute))
      {
        printf("SetConsoleTextAttribute() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::Default;
      ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::Default;
      ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::Default;

      //foreground
      switch(iForeground)
      {
      case Black:
        ansi_foreground = ansi::ForegroundColor::Black;
        break;
      case White:
        ansi_foreground = ansi::ForegroundColor::White;
        break;
      case DarkBlue:
        ansi_foreground = ansi::ForegroundColor::Blue;
        break;
      case DarkGreen:
        ansi_foreground = ansi::ForegroundColor::Green;
        break;
      case DarkCyan:
        ansi_foreground = ansi::ForegroundColor::Cyan;
        break;
      case DarkRed:
        ansi_foreground = ansi::ForegroundColor::Red;
        break;
      case DarkMagenta:
        ansi_foreground = ansi::ForegroundColor::Magenta;
        break;
      case DarkYellow:
        ansi_foreground = ansi::ForegroundColor::Yellow;
        break;
      case DarkGray:
        ansi_foreground = ansi::ForegroundColor::DarkGray;
        break;
      case Blue:
        ansi_foreground = ansi::ForegroundColor::LightBlue;
        break;
      case Green:
        ansi_foreground = ansi::ForegroundColor::LightGreen;
        break;
      case Cyan:
        ansi_foreground = ansi::ForegroundColor::LightCyan;
        break;
      case Red:
        ansi_foreground = ansi::ForegroundColor::LightRed;
        break;
      case Magenta:
        ansi_foreground = ansi::ForegroundColor::LightMagenta;
        break;
      case Yellow:
        ansi_foreground = ansi::ForegroundColor::LightYellow;
        break;
      case Gray:
        ansi_foreground = ansi::ForegroundColor::LightGray;
        break;
      };

      //background
      switch(iBackground)
      {
      case Black:
        ansi_background = ansi::BackgroundColor::Black;
        break;
      case White:
        ansi_background = ansi::BackgroundColor::White;
        break;
      case DarkBlue:
        ansi_background = ansi::BackgroundColor::Blue;
        break;
      case DarkGreen:
        ansi_background = ansi::BackgroundColor::Green;
        break;
      case DarkCyan:
        ansi_background = ansi::BackgroundColor::Cyan;
        break;
      case DarkRed:
        ansi_background = ansi::BackgroundColor::Red;
        break;
      case DarkMagenta:
        ansi_background = ansi::BackgroundColor::Magenta;
        break;
      case DarkYellow:
        ansi_background = ansi::BackgroundColor::Yellow;
        break;
      case DarkGray:
        ansi_background = ansi::BackgroundColor::DarkGray;
        break;
      case Blue:
        ansi_background = ansi::BackgroundColor::LightBlue;
        break;
      case Green:
        ansi_background = ansi::BackgroundColor::LightGreen;
        break;
      case Cyan:
        ansi_background = ansi::BackgroundColor::LightCyan;
        break;
      case Red:
        ansi_background = ansi::BackgroundColor::LightRed;
        break;
      case Magenta:
        ansi_background = ansi::BackgroundColor::LightMagenta;
        break;
      case Yellow:
        ansi_background = ansi::BackgroundColor::LightYellow;
        break;
      case Gray:
        ansi_background = ansi::BackgroundColor::LightGray;
        break;
      };

      printf("\033[%d;%d;%dm", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
      //printf("{%d;%d;%d}", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
#endif
    }

    void getTextColor(TextColor & oForeground, TextColor & oBackground)
    {
      oForeground = Gray;
      oBackground = Black;

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE)
      {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
      {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }

      DWORD foregroundInfo = csbiInfo.wAttributes & (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      DWORD backgroundInfo = csbiInfo.wAttributes & (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

      //foreground
      switch(foregroundInfo)
      {
      case 0:
        oForeground = Black;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = White;
        break;
      case FOREGROUND_BLUE:
        oForeground = DarkBlue;
        break;
      case FOREGROUND_GREEN:
        oForeground = DarkGreen;
        break;
      case FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = DarkCyan;
        break;
      case FOREGROUND_RED:
        oForeground = DarkRed;
        break;
      case FOREGROUND_RED | FOREGROUND_BLUE:
        oForeground = DarkMagenta;
        break;
      case FOREGROUND_RED | FOREGROUND_GREEN:
        oForeground = DarkYellow;
        break;
      case FOREGROUND_INTENSITY:
        oForeground = DarkGray;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_BLUE:
        oForeground = Blue;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_GREEN:
        oForeground = Green;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = Cyan;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED:
        oForeground = Red;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE:
        oForeground = Magenta;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN:
        oForeground = Yellow;
        break;
      case FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = Gray;
        break;
      };

      //background
      switch(backgroundInfo)
      {
      case 0:
        oBackground = Black;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = White;
        break;
      case BACKGROUND_BLUE:
        oBackground = DarkBlue;
        break;
      case BACKGROUND_GREEN:
        oBackground = DarkGreen;
        break;
      case BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = DarkCyan;
        break;
      case BACKGROUND_RED:
        oBackground = DarkRed;
        break;
      case BACKGROUND_RED | BACKGROUND_BLUE:
        oBackground = DarkMagenta;
        break;
      case BACKGROUND_RED | BACKGROUND_GREEN:
        oBackground = DarkYellow;
        break;
      case BACKGROUND_INTENSITY:
        oBackground = DarkGray;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_BLUE:
        oBackground = Blue;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_GREEN:
        oBackground = Green;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = Cyan;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED:
        oBackground = Red;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE:
        oBackground = Magenta;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN:
        oBackground = Yellow;
        break;
      case BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = Gray;
        break;
      };
#elif __linux__
      //not implemented yet
      oForeground = Gray;
      oBackground = Black;
#endif
    }

    void setDefaultTextColor()
    {
#ifdef _WIN32
      ra::console::setTextColor(ra::console::Gray, ra::console::Black);
#elif __linux__
      printf("\033[0m");
#endif
    }

    bool isDesktopGuiAvailable()
    {
#ifdef _WIN32
      return true;
#elif __linux__
      std::string display = ra::environment::getEnvironmentVariable("DISPLAY");
      bool hasDesktopGui = !display.empty();
      return hasDesktopGui;
#endif
    }

    bool isRunFromDesktop()
    {
#ifdef _WIN32
      std::string prompt = ra::environment::getEnvironmentVariable("PROMPT");
      bool hasNoPrompt = prompt.empty();
      return hasNoPrompt;
#elif __linux__
      //https://stackoverflow.com/questions/13204177/how-to-find-out-if-running-from-terminal-or-gui
      if (isatty(fileno(stdin)))
        return false;
      else
        return true;
#endif
    }

    bool hasConsoleOwnership()
    {
#ifdef _WIN32
      //https://stackoverflow.com/questions/9009333/how-to-check-if-the-program-is-run-from-a-console
      HWND hConsoleWnd = GetConsoleWindow();
      if (hConsoleWnd == NULL)
      {
        printf("Failed calling GetConsoleWindow(). Error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return false;
      }
      DWORD dwConsoleProcessId = 0;
      if (!GetWindowThreadProcessId(hConsoleWnd, &dwConsoleProcessId))
      {
        printf("Failed calling GetWindowThreadProcessId(). Error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return false;
      }
      DWORD dwCurrentProcessId = GetCurrentProcessId();
      if (dwCurrentProcessId == dwConsoleProcessId)
      {
        //the current process is the process that created this console.
        return true;
      }

      //another process created the console (ie: cmd.exe)
      return false;
#elif __linux__
      return isRunFromDesktop();
#endif
    }

  } //namespace console
} //namespace ra
