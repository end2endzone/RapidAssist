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
#include "rapidassist/timing.h"
#include "rapidassist/environment.h"
#include <vector>

#ifdef _WIN32

#include <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif /* WIN32_LEAN_AND_MEAN */
#include <Windows.h>

#elif __linux__

#include <cstdio>       // fileno()
#include <unistd.h>     // isatty(), read() and STDIN_FILENO
#include <sys/ioctl.h>  //for ioctl() and FIONREAD
#include <termios.h>    //for tcgetattr() and tcsetattr() 
#include <fstream>      //for exit()
#include <fcntl.h>      //for fcntl(), F_GETFL and F_SETFL

//for getCursorPos()
#include <stdlib.h>
#include <string.h>

#endif

namespace ra
{
  namespace console
  {
    struct CursorCoordinate {
      int x;
      int y;
    };

    // Keep track of the cursor position for pushCursorPos() and popCursorPos()
    std::vector<CursorCoordinate> cursor_position_stack;

    void getCursorPos(int & col, int & row) {
      col = 0;
      row = 0;

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
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
      int loop_count = 0;
      bool success = false;
      char buf[32];
      while(!success && loop_count < MAX_LOOP_COUNT) {
        ssize_t num_write = write(0, "\033[6n", 4);
        fflush(stdout);

        ssize_t num_read = 0;
        if (num_write == 4)
          num_read = read(0, buf, sizeof(buf)-1);

        //validate format \033[63;1R
        if (num_write == 4 && num_read >= 6 && buf[0] == '\033' && buf[num_read - 1] == 'R') {
          buf[0] = '!'; //in case we want to print buf for debugging
          buf[num_read] = '\0';

          //try to parse the result
          const char * last_char = &buf[num_read];
          const char * next_char = &buf[1];

          if (next_char[0] == '[')
            next_char++; //next character
          else
            next_char = last_char; //fail parsing. jump to the end of the string

          //parse row
          row = 0;
          while (next_char[0] >= '0' && next_char[0] <= '9') {
            row = 10 * row + next_char[0] - '0';
            next_char++; //next character
          }

          if (next_char[0] == ';')
            next_char++; //next character
          else
            next_char = last_char; //fail parsing. jump to the end of the string

          //parse col
          col = 0;
          while (next_char[0] >= '0' && next_char[0] <= '9') {
            col = 10 * col + next_char[0] - '0';
            next_char++; //next character
          }

          if (next_char[0] == 'R')
            success = true;
        }

        loop_count++;
      }

      tcsetattr(0, TCSANOW, &torig);

      if (success) {
        col--; //convert from ANSI 1-based to 0-based
      }
#endif
    }

    void setCursorPos(const int & col, const int & row) {
#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      COORD coord;
      coord.X = col;
      coord.Y = row;
      if (!SetConsoleCursorPosition(hStdout, coord)) {
        printf("SetConsoleCursorPosition() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      printf("\033[%d;%dH", row, col);
#endif
    }

    void getDimension(int & width, int & height) {
      width = 0;
      height = 0;
#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
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

#ifdef _WIN32
    char my_getch() {
      char c = (char)_getch();
      return c;
    }

    int my_kbhit() {
      int hits = _kbhit();
      return hits;
    }

#else
    /**
     * Set a file descriptor to blocking or non-blocking mode.
     * http://code.activestate.com/recipes/577384-setting-a-file-descriptor-to-blocking-or-non-block/
     *
     * @param fd The file descriptor
     * @param blocking 0:non-blocking mode, 1:blocking mode
     *
     * @return 1:success, 0:failure.
     **/
    int fd_set_blocking(int fd, int blocking) {
      /* Save the current flags */
      int flags = fcntl(fd, F_GETFL, 0);
      if (flags == -1)
        return 0;

      if (blocking)
        flags &= ~O_NONBLOCK;
      else
        flags |= O_NONBLOCK;
      return fcntl(fd, F_SETFL, flags) != -1;
    }

    //https://stackoverflow.com/questions/9429138/non-blocking-keyboard-read-c-c/28222851
    char my_getch() {
      int kfd = STDIN_FILENO;
      struct termios cooked, raw;
      char c;

      // remember console settings to get back to a 'sane' configuration
      if (tcgetattr(kfd, &cooked) < 0) {
        perror("tcgetattr()");
        return '\0';
      }

      // set the console in raw mode
      memcpy(&raw, &cooked, sizeof(struct termios));
      raw.c_lflag &= ~(ICANON | ECHO);
      if (tcsetattr(kfd, TCSANOW, &raw) < 0) {
        perror("tcsetattr()");

        // reset console to its original mode before leaving the function
        tcsetattr(kfd, TCSANOW, &cooked);

        return '\0';
      }

      // get the next event from the keyboard
      if (read(kfd, &c, 1) < 0) {
        perror("read():");

        // reset console to its original mode before leaving the function
        tcsetattr(kfd, TCSANOW, &cooked);

        return '\0';
      }

      // reset console to its original mode before leaving the function
      if (tcsetattr(kfd, TCSANOW, &cooked) < 0)
        perror("tcsetattr()");

      return c;
    }

    int my_kbhit() {
      int kfd = STDIN_FILENO;
      struct termios cooked, raw;
      char c;

      // remember console settings to get back to previous configuration
      if (tcgetattr(kfd, &cooked) < 0)
        perror("tcgetattr()");

      // turn off line buffering
      memcpy(&raw, &cooked, sizeof(struct termios));
      raw.c_lflag &= ~(ICANON);
      if (tcsetattr(kfd, TCSANOW, &raw) < 0) {
        perror("tcsetattr()");

        // reset console to its original mode before leaving the function
        tcsetattr(kfd, TCSANOW, &cooked);

        return 0;
      }

      // get the number of bytes waiting to be readed
      int bytes_waiting = 0;
      if (ioctl(kfd, FIONREAD, &bytes_waiting) < 0) {
        perror("tcsetattr()");

        // reset console to its original mode before leaving the function
        tcsetattr(kfd, TCSANOW, &cooked);

        return 0;
      }

      // reset console to its original mode before leaving the function
      if (tcsetattr(kfd, TCSANOW, &cooked) < 0)
        perror("tcsetattr()");

      return bytes_waiting;
    }
#endif

    int waitKeyPress() {
      //empty input buffer first
      int key = 0;
      while (my_kbhit()) {
        key = my_getch();
      }

      //wait for a new keypress
      key = my_getch();
      return key;
    }

    void clearScreen() {
      //system("cls");

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      COORD coord = { 0 , 0 };
      DWORD number_of_chars_written;
      DWORD length;
      length = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
      if (!FillConsoleOutputCharacter(hStdout, TEXT(' '), length, coord, &number_of_chars_written)) {
        printf("FillConsoleOutputCharacter() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!FillConsoleOutputAttribute(hStdout, csbiInfo.wAttributes, length, coord, &number_of_chars_written)) {
        printf("FillConsoleOutputAttribute() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      if (!SetConsoleCursorPosition(hStdout, coord)) {
        printf("SetConsoleCursorPosition() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      //not implemented yet
      printf("\033[2J");
#endif
    }

    void pushCursorPos() {
      CursorCoordinate coord;
      getCursorPos(coord.x, coord.y);

      cursor_position_stack.push_back(coord);
    }

    void popCursorPos() {
      if (!cursor_position_stack.empty()) {
        size_t offset = cursor_position_stack.size() - 1;
        const CursorCoordinate & last = cursor_position_stack[offset];
        setCursorPos(last.x, last.y);

        //remove the last entry
        cursor_position_stack.erase(cursor_position_stack.begin() + offset);
      }
    }

    char getAnimationSprite(double iRefreshRate) {
      static const char animation_sprites[] = {'-', '\\', '|', '/'};
      static const int num_animation_sprites = sizeof(animation_sprites)/sizeof(animation_sprites[0]);
      double seconds = ra::timing::getMillisecondsTimer(); //already seconds
      int sprite_index = (int)(seconds/iRefreshRate);
      sprite_index = sprite_index % num_animation_sprites;
      char sprite = animation_sprites[sprite_index];
      return sprite;
    }

    void printAnimationCursor() {
      pushCursorPos();
      printf("%c", getAnimationSprite(0.15));
      popCursorPos();
    }

    const char * getTextColorName(const TextColor & color) {
      switch (color) {
      case BLACK:
        return "Black";
        break;
      case WHITE:
        return "White";
        break;
      case DARK_BLUE:
        return "DarkBlue";
        break;
      case DARK_GREEN:
        return "DarkGreen";
        break;
      case DARK_CYAN:
        return "DarkCyan";
        break;
      case DARK_RED:
        return "DarkRed";
        break;
      case DARK_MAGENTA:
        return "DarkMagenta";
        break;
      case DARK_YELLOW:
        return "DarkYellow";
        break;
      case DARK_GRAY:
        return "DarkGray";
        break;
      case BLUE:
        return "Blue";
        break;
      case GREEN:
        return "Green";
        break;
      case CYAN:
        return "Cyan";
        break;
      case RED:
        return "Red";
        break;
      case MAGENTA:
        return "Magenta";
        break;
      case YELLOW:
        return "Yellow";
        break;
      case GRAY:
        return "Gray";
        break;
      default:
        return "";
      };
    }

    const char * ansi::FormatAttribute::toString(const ansi::FormatAttribute::Attr & attr) {
      switch(attr) {
      case ansi::FormatAttribute::DEFAULT    : return "Default"   ; break;
      case ansi::FormatAttribute::BOLD       : return "Bold"      ; break;
      case ansi::FormatAttribute::DIM        : return "Dim"       ; break;
      case ansi::FormatAttribute::UNDERLINED : return "Underlined"; break;
      case ansi::FormatAttribute::BLINK      : return "Blink"     ; break;
      case ansi::FormatAttribute::REVERSE    : return "Reverse"   ; break;
      case ansi::FormatAttribute::HIDDEN     : return "Hidden"    ; break;
      };
      return "Unknown";
    }

    const char * ansi::ForegroundColor::toString(const ansi::ForegroundColor::Color & color) {
      switch(color) {
      case ansi::ForegroundColor::DEFAULT      : return "Default"      ; break;
      case ansi::ForegroundColor::BLACK        : return "Black"        ; break;
      case ansi::ForegroundColor::RED          : return "Red"          ; break;
      case ansi::ForegroundColor::GREEN        : return "Green"        ; break;
      case ansi::ForegroundColor::YELLOW       : return "Yellow"       ; break;
      case ansi::ForegroundColor::BLUE         : return "Blue"         ; break;
      case ansi::ForegroundColor::MAGENTA      : return "Magenta"      ; break;
      case ansi::ForegroundColor::CYAN         : return "Cyan"         ; break;
      case ansi::ForegroundColor::LIGHT_GRAY   : return "Light_Gray"   ; break;
      case ansi::ForegroundColor::DARK_GRAY    : return "Dark_Gray"    ; break;
      case ansi::ForegroundColor::LIGHT_RED    : return "Light_Red"    ; break;
      case ansi::ForegroundColor::LIGHT_GREEN  : return "Light_Green"  ; break;
      case ansi::ForegroundColor::LIGHT_YELLOW : return "Light_Yellow" ; break;
      case ansi::ForegroundColor::LIGHT_BLUE   : return "Light_Blue"   ; break;
      case ansi::ForegroundColor::LIGHT_MAGENTA: return "Light_Magenta"; break;
      case ansi::ForegroundColor::LIGHT_CYAN   : return "Light_Cyan"   ; break;
      case ansi::ForegroundColor::WHITE        : return "White"        ; break;
      };
      return "Unknown";
    }

    const char * ansi::BackgroundColor::toString(const ansi::BackgroundColor::Color & color) {
      switch(color) {
      case ansi::BackgroundColor::DEFAULT      : return "Default"      ; break;
      case ansi::BackgroundColor::BLACK        : return "Black"        ; break;
      case ansi::BackgroundColor::RED          : return "Red"          ; break;
      case ansi::BackgroundColor::GREEN        : return "Green"        ; break;
      case ansi::BackgroundColor::YELLOW       : return "Yellow"       ; break;
      case ansi::BackgroundColor::BLUE         : return "Blue"         ; break;
      case ansi::BackgroundColor::MAGENTA      : return "Magenta"      ; break;
      case ansi::BackgroundColor::CYAN         : return "Cyan"         ; break;
      case ansi::BackgroundColor::LIGHT_GRAY   : return "Light_Gray"   ; break;
      case ansi::BackgroundColor::DARK_GRAY    : return "Dark_Gray"    ; break;
      case ansi::BackgroundColor::LIGHT_RED    : return "Light_Red"    ; break;
      case ansi::BackgroundColor::LIGHT_GREEN  : return "Light_Green"  ; break;
      case ansi::BackgroundColor::LIGHT_YELLOW : return "Light_Yellow" ; break;
      case ansi::BackgroundColor::LIGHT_BLUE   : return "Light_Blue"   ; break;
      case ansi::BackgroundColor::LIGHT_MAGENTA: return "Light_Magenta"; break;
      case ansi::BackgroundColor::LIGHT_CYAN   : return "Light_Cyan"   ; break;
      case ansi::BackgroundColor::WHITE        : return "White"        ; break;
      };
      return "Unknown";
    }

    void setTextColor(const TextColor & iForeground, const TextColor & iBackground) {
#ifdef _WIN32
      WORD foreground_attribute = 0;
      switch (iForeground) {
      case BLACK:
        foreground_attribute = 0;
        break;
      case WHITE:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DARK_BLUE:
        foreground_attribute = FOREGROUND_BLUE;
        break;
      case DARK_GREEN:
        foreground_attribute = FOREGROUND_GREEN;
        break;
      case DARK_CYAN:
        foreground_attribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case DARK_RED:
        foreground_attribute = FOREGROUND_RED;
        break;
      case DARK_MAGENTA:
        foreground_attribute = FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case DARK_YELLOW:
        foreground_attribute = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case DARK_GRAY:
        foreground_attribute = FOREGROUND_INTENSITY;
        break;
      case BLUE:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
        break;
      case GREEN:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        break;
      case CYAN:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      case RED:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
      case MAGENTA:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
        break;
      case YELLOW:
        foreground_attribute = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        break;
      case GRAY:
        foreground_attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
      };

      WORD background_attribute = 0;
      switch (iBackground) {
      case BLACK:
        background_attribute = 0;
        break;
      case WHITE:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DARK_BLUE:
        background_attribute = BACKGROUND_BLUE;
        break;
      case DARK_GREEN:
        background_attribute = BACKGROUND_GREEN;
        break;
      case DARK_CYAN:
        background_attribute = BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case DARK_RED:
        background_attribute = BACKGROUND_RED;
        break;
      case DARK_MAGENTA:
        background_attribute = BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case DARK_YELLOW:
        background_attribute = BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case DARK_GRAY:
        background_attribute = BACKGROUND_INTENSITY;
        break;
      case BLUE:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_BLUE;
        break;
      case GREEN:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN;
        break;
      case CYAN:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      case RED:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_RED;
        break;
      case MAGENTA:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
        break;
      case YELLOW:
        background_attribute = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
        break;
      case GRAY:
        background_attribute = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        break;
      };

      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }

      if (!SetConsoleTextAttribute(hStdout, foreground_attribute | background_attribute)) {
        printf("SetConsoleTextAttribute() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
#elif __linux__
      ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::DEFAULT;
      ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::DEFAULT;
      ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::DEFAULT;

      //foreground
      switch (iForeground) {
      case Black:
        ansi_foreground = ansi::ForegroundColor::BLACK;
        break;
      case White:
        ansi_foreground = ansi::ForegroundColor::WHITE;
        break;
      case DarkBlue:
        ansi_foreground = ansi::ForegroundColor::BLUE;
        break;
      case DarkGreen:
        ansi_foreground = ansi::ForegroundColor::GREEN;
        break;
      case DarkCyan:
        ansi_foreground = ansi::ForegroundColor::CYAN;
        break;
      case DarkRed:
        ansi_foreground = ansi::ForegroundColor::RED;
        break;
      case DarkMagenta:
        ansi_foreground = ansi::ForegroundColor::MAGENTA;
        break;
      case DarkYellow:
        ansi_foreground = ansi::ForegroundColor::YELLOW;
        break;
      case DarkGray:
        ansi_foreground = ansi::ForegroundColor::DARK_GRAY;
        break;
      case Blue:
        ansi_foreground = ansi::ForegroundColor::LIGHT_BLUE;
        break;
      case Green:
        ansi_foreground = ansi::ForegroundColor::LIGHT_GREEN;
        break;
      case Cyan:
        ansi_foreground = ansi::ForegroundColor::LIGHT_CYAN;
        break;
      case Red:
        ansi_foreground = ansi::ForegroundColor::LIGHT_RED;
        break;
      case Magenta:
        ansi_foreground = ansi::ForegroundColor::LIGHT_MAGENTA;
        break;
      case Yellow:
        ansi_foreground = ansi::ForegroundColor::LIGHT_YELLOW;
        break;
      case Gray:
        ansi_foreground = ansi::ForegroundColor::LIGHT_GRAY;
        break;
      };

      //background
      switch (iBackground) {
      case Black:
        ansi_background = ansi::BackgroundColor::BLACK;
        break;
      case White:
        ansi_background = ansi::BackgroundColor::WHITE;
        break;
      case DarkBlue:
        ansi_background = ansi::BackgroundColor::BLUE;
        break;
      case DarkGreen:
        ansi_background = ansi::BackgroundColor::GREEN;
        break;
      case DarkCyan:
        ansi_background = ansi::BackgroundColor::CYAN;
        break;
      case DarkRed:
        ansi_background = ansi::BackgroundColor::RED;
        break;
      case DarkMagenta:
        ansi_background = ansi::BackgroundColor::MAGENTA;
        break;
      case DarkYellow:
        ansi_background = ansi::BackgroundColor::YELLOW;
        break;
      case DarkGray:
        ansi_background = ansi::BackgroundColor::DARK_GRAY;
        break;
      case Blue:
        ansi_background = ansi::BackgroundColor::LIGHT_BLUE;
        break;
      case Green:
        ansi_background = ansi::BackgroundColor::LIGHT_GREEN;
        break;
      case Cyan:
        ansi_background = ansi::BackgroundColor::LIGHT_CYAN;
        break;
      case Red:
        ansi_background = ansi::BackgroundColor::LIGHT_RED;
        break;
      case Magenta:
        ansi_background = ansi::BackgroundColor::LIGHT_MAGENTA;
        break;
      case Yellow:
        ansi_background = ansi::BackgroundColor::LIGHT_YELLOW;
        break;
      case Gray:
        ansi_background = ansi::BackgroundColor::LIGHT_GRAY;
        break;
      };

      printf("\033[%d;%d;%dm", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
      //printf("{%d;%d;%d}", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
#endif
    }

    void getTextColor(TextColor & oForeground, TextColor & oBackground) {
      oForeground = GRAY;
      oBackground = BLACK;

#ifdef _WIN32
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
      if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }
      CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
      if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
        printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return;
      }

      DWORD foreground_info = csbiInfo.wAttributes & (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      DWORD background_info = csbiInfo.wAttributes & (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

      //foreground
      switch(foreground_info) {
      case 0:
        oForeground = BLACK;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = WHITE;
        break;
      case FOREGROUND_BLUE:
        oForeground = DARK_BLUE;
        break;
      case FOREGROUND_GREEN:
        oForeground = DARK_GREEN;
        break;
      case FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = DARK_CYAN;
        break;
      case FOREGROUND_RED:
        oForeground = DARK_RED;
        break;
      case FOREGROUND_RED | FOREGROUND_BLUE:
        oForeground = DARK_MAGENTA;
        break;
      case FOREGROUND_RED | FOREGROUND_GREEN:
        oForeground = DARK_YELLOW;
        break;
      case FOREGROUND_INTENSITY:
        oForeground = DARK_GRAY;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_BLUE:
        oForeground = BLUE;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_GREEN:
        oForeground = GREEN;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = CYAN;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED:
        oForeground = RED;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE:
        oForeground = MAGENTA;
        break;
      case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN:
        oForeground = YELLOW;
        break;
      case FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
        oForeground = GRAY;
        break;
      };

      //background
      switch(background_info) {
      case 0:
        oBackground = BLACK;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = WHITE;
        break;
      case BACKGROUND_BLUE:
        oBackground = DARK_BLUE;
        break;
      case BACKGROUND_GREEN:
        oBackground = DARK_GREEN;
        break;
      case BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = DARK_CYAN;
        break;
      case BACKGROUND_RED:
        oBackground = DARK_RED;
        break;
      case BACKGROUND_RED | BACKGROUND_BLUE:
        oBackground = DARK_MAGENTA;
        break;
      case BACKGROUND_RED | BACKGROUND_GREEN:
        oBackground = DARK_YELLOW;
        break;
      case BACKGROUND_INTENSITY:
        oBackground = DARK_GRAY;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_BLUE:
        oBackground = BLUE;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_GREEN:
        oBackground = GREEN;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = CYAN;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED:
        oBackground = RED;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE:
        oBackground = MAGENTA;
        break;
      case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN:
        oBackground = YELLOW;
        break;
      case BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
        oBackground = GRAY;
        break;
      };
#elif __linux__
      //not implemented yet
      oForeground = Gray;
      oBackground = Black;
#endif
    }

    void setDefaultTextColor() {
#ifdef _WIN32
      ra::console::setTextColor(ra::console::GRAY, ra::console::BLACK);
#elif __linux__
      printf("\033[0m");
#endif
    }

    bool isDesktopGuiAvailable() {
#ifdef _WIN32
      return true;
#elif __linux__
      std::string display = ra::environment::getEnvironmentVariable("DISPLAY");
      bool has_desktop_gui = !display.empty();
      return has_desktop_gui;
#endif
    }

    bool isRunFromDesktop() {
#ifdef _WIN32
      std::string prompt = ra::environment::getEnvironmentVariable("PROMPT");
      bool has_no_prompt = prompt.empty();
      return has_no_prompt;
#elif __linux__
      //https://stackoverflow.com/questions/13204177/how-to-find-out-if-running-from-terminal-or-gui
      if (isatty(fileno(stdin)))
        return false;
      else
        return true;
#endif
    }

    bool hasConsoleOwnership() {
#ifdef _WIN32
      //https://stackoverflow.com/questions/9009333/how-to-check-if-the-program-is-run-from-a-console
      HWND hConsoleWnd = GetConsoleWindow();
      if (hConsoleWnd == NULL) {
        printf("Failed calling GetConsoleWindow(). Error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return false;
      }
      DWORD console_process_id = 0;
      if (!GetWindowThreadProcessId(hConsoleWnd, &console_process_id)) {
        printf("Failed calling GetWindowThreadProcessId(). Error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
        return false;
      }
      DWORD dwCurrentProcessId = GetCurrentProcessId();
      if (dwCurrentProcessId == console_process_id) {
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
