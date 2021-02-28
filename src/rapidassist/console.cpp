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
#include "rapidassist/undef_windows_macros.h"

#elif defined(__linux__) || defined(__APPLE__)

#include <cstdio>       // fileno()
#include <unistd.h>     // isatty(), read() and STDIN_FILENO
#include <sys/ioctl.h>  //for ioctl() and FIONREAD
#include <termios.h>    //for tcgetattr() and tcsetattr() 
#include <fstream>      //for exit()
#include <fcntl.h>      //for fcntl(), F_GETFL and F_SETFL

//for GetCursorPos()
#include <stdlib.h>
#include <string.h>

#endif

namespace ra { namespace console {

  struct CursorCoordinate {
    int x;
    int y;
  };

  // Keep track of the cursor position for PushCursorPos() and PopCursorPos()
  std::vector<CursorCoordinate> cursor_position_stack;

  void GetCursorPos(int & col, int & row) {
    col = 0;
    row = 0;

#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
      printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
      printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    col = csbi.dwCursorPosition.X;
    row = csbi.dwCursorPosition.Y;
#elif defined(__linux__) || defined(__APPLE__)
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
    while (!success && loop_count < MAX_LOOP_COUNT) {
      ssize_t num_write = write(0, "\033[6n", 4);
      fflush(stdout);

      ssize_t num_read = 0;
      if (num_write == 4)
        num_read = read(0, buf, sizeof(buf) - 1);

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

  void SetCursorPos(const int & col, const int & row) {
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
#elif defined(__linux__) || defined(__APPLE__)
    printf("\033[%d;%dH", row, col);
#endif
  }

  void GetDimension(int & width, int & height) {
    width = 0;
    height = 0;
#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
      printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
      printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    width = (int)csbi.dwMaximumWindowSize.X;
    height = (int)csbi.dwMaximumWindowSize.Y;
#elif defined(__linux__) || defined(__APPLE__)
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

  int WaitKeyPress() {
    //empty input buffer first
    int key = 0;
    while (my_kbhit()) {
      key = my_getch();
    }

    //wait for a new keypress
    key = my_getch();
    return key;
  }

  void ClearScreen() {
    //system("cls");

#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
      printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
      printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    COORD coord = { 0 , 0 };
    DWORD number_of_chars_written;
    DWORD length;
    length = csbi.dwSize.X * csbi.dwSize.Y;
    if (!FillConsoleOutputCharacter(hStdout, TEXT(' '), length, coord, &number_of_chars_written)) {
      printf("FillConsoleOutputCharacter() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
      printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    if (!FillConsoleOutputAttribute(hStdout, csbi.wAttributes, length, coord, &number_of_chars_written)) {
      printf("FillConsoleOutputAttribute() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    if (!SetConsoleCursorPosition(hStdout, coord)) {
      printf("SetConsoleCursorPosition() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
#elif defined(__linux__) || defined(__APPLE__)
    //not implemented yet
    printf("\033[2J");
#endif
  }

  void PushCursorPos() {
    CursorCoordinate coord;
    GetCursorPos(coord.x, coord.y);

    cursor_position_stack.push_back(coord);
  }

  void PopCursorPos() {
    if (!cursor_position_stack.empty()) {
      size_t offset = cursor_position_stack.size() - 1;
      const CursorCoordinate & last = cursor_position_stack[offset];
      SetCursorPos(last.x, last.y);

      //remove the last entry
      cursor_position_stack.erase(cursor_position_stack.begin() + offset);
    }
  }

  char GetAnimationSprite(double refresh_rate) {
    static const char animation_sprites[] = { '-', '\\', '|', '/' };
    static const int num_animation_sprites = sizeof(animation_sprites) / sizeof(animation_sprites[0]);
    double seconds = ra::timing::GetMillisecondsTimer(); //already seconds
    int sprite_index = (int)(seconds / refresh_rate);
    sprite_index = sprite_index % num_animation_sprites;
    char sprite = animation_sprites[sprite_index];
    return sprite;
  }

  void PrintAnimationCursor() {
    printf("%c\b", GetAnimationSprite(0.15));
  }

  const char * GetTextColorName(const TextColor & color) {
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

  const char * ansi::FormatAttribute::ToString(const ansi::FormatAttribute::Attr & attr) {
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

  const char * ansi::ForegroundColor::ToString(const ansi::ForegroundColor::Color & color) {
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

  const char * ansi::BackgroundColor::ToString(const ansi::BackgroundColor::Color & color) {
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

  void SetTextColor(const TextColor & foreground, const TextColor & background) {
#ifdef _WIN32
    WORD foreground_attribute = 0;
    switch (foreground) {
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
    switch (background) {
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
#elif defined(__linux__) || defined(__APPLE__)
    ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::DEFAULT;
    ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::DEFAULT;
    ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::DEFAULT;

    //foreground
    switch (foreground) {
    case BLACK:
      ansi_foreground = ansi::ForegroundColor::BLACK;
      break;
    case WHITE:
      ansi_foreground = ansi::ForegroundColor::WHITE;
      break;
    case DARK_BLUE:
      ansi_foreground = ansi::ForegroundColor::BLUE;
      break;
    case DARK_GREEN:
      ansi_foreground = ansi::ForegroundColor::GREEN;
      break;
    case DARK_CYAN:
      ansi_foreground = ansi::ForegroundColor::CYAN;
      break;
    case DARK_RED:
      ansi_foreground = ansi::ForegroundColor::RED;
      break;
    case DARK_MAGENTA:
      ansi_foreground = ansi::ForegroundColor::MAGENTA;
      break;
    case DARK_YELLOW:
      ansi_foreground = ansi::ForegroundColor::YELLOW;
      break;
    case DARK_GRAY:
      ansi_foreground = ansi::ForegroundColor::DARK_GRAY;
      break;
    case BLUE:
      ansi_foreground = ansi::ForegroundColor::LIGHT_BLUE;
      break;
    case GREEN:
      ansi_foreground = ansi::ForegroundColor::LIGHT_GREEN;
      break;
    case CYAN:
      ansi_foreground = ansi::ForegroundColor::LIGHT_CYAN;
      break;
    case RED:
      ansi_foreground = ansi::ForegroundColor::LIGHT_RED;
      break;
    case MAGENTA:
      ansi_foreground = ansi::ForegroundColor::LIGHT_MAGENTA;
      break;
    case YELLOW:
      ansi_foreground = ansi::ForegroundColor::LIGHT_YELLOW;
      break;
    case GRAY:
      ansi_foreground = ansi::ForegroundColor::LIGHT_GRAY;
      break;
    };

    //background
    switch (background) {
    case BLACK:
      ansi_background = ansi::BackgroundColor::BLACK;
      break;
    case WHITE:
      ansi_background = ansi::BackgroundColor::WHITE;
      break;
    case DARK_BLUE:
      ansi_background = ansi::BackgroundColor::BLUE;
      break;
    case DARK_GREEN:
      ansi_background = ansi::BackgroundColor::GREEN;
      break;
    case DARK_CYAN:
      ansi_background = ansi::BackgroundColor::CYAN;
      break;
    case DARK_RED:
      ansi_background = ansi::BackgroundColor::RED;
      break;
    case DARK_MAGENTA:
      ansi_background = ansi::BackgroundColor::MAGENTA;
      break;
    case DARK_YELLOW:
      ansi_background = ansi::BackgroundColor::YELLOW;
      break;
    case DARK_GRAY:
      ansi_background = ansi::BackgroundColor::DARK_GRAY;
      break;
    case BLUE:
      ansi_background = ansi::BackgroundColor::LIGHT_BLUE;
      break;
    case GREEN:
      ansi_background = ansi::BackgroundColor::LIGHT_GREEN;
      break;
    case CYAN:
      ansi_background = ansi::BackgroundColor::LIGHT_CYAN;
      break;
    case RED:
      ansi_background = ansi::BackgroundColor::LIGHT_RED;
      break;
    case MAGENTA:
      ansi_background = ansi::BackgroundColor::LIGHT_MAGENTA;
      break;
    case YELLOW:
      ansi_background = ansi::BackgroundColor::LIGHT_YELLOW;
      break;
    case GRAY:
      ansi_background = ansi::BackgroundColor::LIGHT_GRAY;
      break;
    };

    printf("\033[%d;%d;%dm", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
    //printf("{%d;%d;%d}", (int)ansi_attr, (int)ansi_foreground, (int)ansi_background);
#endif
  }

  void GetTextColor(TextColor & foreground, TextColor & background) {
    foreground = GRAY;
    background = BLACK;

#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
      printf("GetStdHandle() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
      printf("GetConsoleScreenBufferInfo() error: (%d), function '%s', line %d\n", GetLastError(), __FUNCTION__, __LINE__);
      return;
    }

    DWORD foreground_info = csbi.wAttributes & (FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    DWORD background_info = csbi.wAttributes & (BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

    //foreground
    switch (foreground_info) {
    case 0:
      foreground = BLACK;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
      foreground = WHITE;
      break;
    case FOREGROUND_BLUE:
      foreground = DARK_BLUE;
      break;
    case FOREGROUND_GREEN:
      foreground = DARK_GREEN;
      break;
    case FOREGROUND_GREEN | FOREGROUND_BLUE:
      foreground = DARK_CYAN;
      break;
    case FOREGROUND_RED:
      foreground = DARK_RED;
      break;
    case FOREGROUND_RED | FOREGROUND_BLUE:
      foreground = DARK_MAGENTA;
      break;
    case FOREGROUND_RED | FOREGROUND_GREEN:
      foreground = DARK_YELLOW;
      break;
    case FOREGROUND_INTENSITY:
      foreground = DARK_GRAY;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_BLUE:
      foreground = BLUE;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_GREEN:
      foreground = GREEN;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE:
      foreground = CYAN;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED:
      foreground = RED;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE:
      foreground = MAGENTA;
      break;
    case FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN:
      foreground = YELLOW;
      break;
    case FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE:
      foreground = GRAY;
      break;
    };

    //background
    switch (background_info) {
    case 0:
      background = BLACK;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
      background = WHITE;
      break;
    case BACKGROUND_BLUE:
      background = DARK_BLUE;
      break;
    case BACKGROUND_GREEN:
      background = DARK_GREEN;
      break;
    case BACKGROUND_GREEN | BACKGROUND_BLUE:
      background = DARK_CYAN;
      break;
    case BACKGROUND_RED:
      background = DARK_RED;
      break;
    case BACKGROUND_RED | BACKGROUND_BLUE:
      background = DARK_MAGENTA;
      break;
    case BACKGROUND_RED | BACKGROUND_GREEN:
      background = DARK_YELLOW;
      break;
    case BACKGROUND_INTENSITY:
      background = DARK_GRAY;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_BLUE:
      background = BLUE;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_GREEN:
      background = GREEN;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE:
      background = CYAN;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED:
      background = RED;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE:
      background = MAGENTA;
      break;
    case BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN:
      background = YELLOW;
      break;
    case BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE:
      background = GRAY;
      break;
    };
#elif defined(__linux__) || defined(__APPLE__)
    //not implemented yet
    foreground = GRAY;
    background = BLACK;
#endif
  }

  void SetDefaultTextColor() {
#ifdef _WIN32
    ra::console::SetTextColor(ra::console::GRAY, ra::console::BLACK);
#elif defined(__linux__) || defined(__APPLE__)
    printf("\033[0m");
#endif
  }

  bool IsDesktopGuiAvailable() {
#ifdef _WIN32
    return true;
#elif defined(__linux__) || defined(__APPLE__)
    std::string display = ra::environment::GetEnvironmentVariable("DISPLAY");
    bool has_desktop_gui = !display.empty();
    return has_desktop_gui;
#endif
  }

  bool IsRunFromDesktop() {
#ifdef _WIN32
    std::string prompt = ra::environment::GetEnvironmentVariable("PROMPT");
    bool has_no_prompt = prompt.empty();
    return has_no_prompt;
#elif defined(__linux__) || defined(__APPLE__)
    //https://stackoverflow.com/questions/13204177/how-to-find-out-if-running-from-terminal-or-gui
    if (isatty(fileno(stdin)))
      return false;
    else
      return true;
#endif
  }

  bool HasConsoleOwnership() {
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
#elif defined(__linux__) || defined(__APPLE__)
    return IsRunFromDesktop();
#endif
  }

} //namespace console
} //namespace ra
