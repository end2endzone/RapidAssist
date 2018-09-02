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
      printf("\033[%d;%dH", y, x);
#endif
    }

    void getDimension(int & width, int & height)
    {
#ifdef _WIN32
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
      width = (int)csbi.dwMaximumWindowSize.X;
      height = (int)csbi.dwMaximumWindowSize.Y;
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

    namespace ansi
    {
      namespace ForegroundColor
      {
        enum Color
        {
          Black = 30,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          White,
        };
      }; //namespace ForegroundColor
      namespace BackgroundColor
      {
        enum Color
        {
          Black = 40,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          White,
        };
      }; //namespace BackgroundColor
      namespace FormatAttribute
      {
        enum Attr
        {
          Default = 0,
          Bold = 1,
          Dim = 2,
          Underlined = 3,
          Blink = 5,
          Reverse = 7,
          Hidden = 8
        };
      };
    }; //namespace ansi
    
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

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foregroundAttribute | backgroundAttribute);
#elif __linux__
      ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::Default;

      ansi::ForegroundColor::Color ansi_foreground;
      ansi::BackgroundColor::Color ansi_background;

      //foreground
      switch(iForeground)
      {
      case Black:
        ansi_foreground = ansi::ForegroundColor::Black;
        break;
      case White:
        ansi_attr = ansi::FormatAttribute::Bold;
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
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Black;
        break;
      case Blue:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Blue;
        break;
      case Green:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Green;
        break;
      case Cyan:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Cyan;
        break;
      case Red:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Red;
        break;
      case Magenta:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Magenta;
        break;
      case Yellow:
        ansi_attr = ansi::FormatAttribute::Bold;
        ansi_foreground = ansi::ForegroundColor::Yellow;
        break;
      case Gray:
        ansi_foreground = ansi::ForegroundColor::White;
        break;
      };

      //background
      switch(iBackground)
      {
      case Black:
        ansi_background = ansi::BackgroundColor::Black;
        break;
      case White:
        ansi_attr = ansi::FormatAttribute::Underlined;
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
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Black;
        break;
      case Blue:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Blue;
        break;
      case Green:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Green;
        break;
      case Cyan:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Cyan;
        break;
      case Red:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Red;
        break;
      case Magenta:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Red;
        break;
      case Yellow:
        ansi_attr = ansi::FormatAttribute::Underlined;
        ansi_background = ansi::BackgroundColor::Yellow;
        break;
      case Gray:
        ansi_background = ansi::BackgroundColor::White;
        break;
      };

      printf("\033[%d;%d;%dm", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
      //printf("{%d;%d;%d}", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
#endif
    }

    void getTextColor(TextColor & oForeground, TextColor & oBackground)
    {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    
    DWORD foregroundInfo = info.wAttributes & (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    DWORD backgroundInfo = info.wAttributes & (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

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
      HWND consoleWnd = GetConsoleWindow();
      DWORD dwConsoleProcessId = 0;
      GetWindowThreadProcessId(consoleWnd, &dwConsoleProcessId);
      DWORD dwCurrentProcessId = GetCurrentProcessId();
      GetWindowThreadProcessId(consoleWnd, &dwConsoleProcessId);
      if (dwCurrentProcessId==dwConsoleProcessId)
      {
        return true;
      }
      return false;
#elif __linux__
      return isRunFromDesktop();
#endif
    }

  } //namespace console
} //namespace ra
