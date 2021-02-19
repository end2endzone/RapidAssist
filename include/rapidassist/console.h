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

#ifndef RA_CONSOLE_H
#define RA_CONSOLE_H

#include "rapidassist/config.h"

namespace ra { namespace console {

  /// <summary>
  /// Returns the cursor position.
  /// </summary>
  /// <remarks>The console x and y coordinate system is 0 based</remarks>
  /// <param name="col">The x coordinate of the cursor.</param>
  /// <param name="row">The y coordinate of the cursor.</param>
  void GetCursorPos(int & col, int & row);

  /// <summary>
  /// Sets the cursor position.
  /// </summary>
  /// <remarks>The console x and y coordinate system is 0 based</remarks>
  /// <param name="col">The x coordinate of the cursor.</param>
  /// <param name="row">The y coordinate of the cursor.</param>
  void SetCursorPos(const int & col, const int & row);

  /// <summary>
  /// Gets the width and height of the console window.
  /// </summary>
  /// <remarks>
  /// On Windows, the function returns the size of the console when maximized (and not the maximum length of a string without carriage return).
  /// The result can be used to 'center' a text on the console.
  /// </remarks>
  /// <param name="width">The width of the console.</param>
  /// <param name="height">The height of the console.</param>
  void GetDimension(int & width, int & height);

  /// <summary>
  /// Clears the screen.
  /// </summary>
  void ClearScreen();

  /// <summary>
  /// Wait for the user to press a key.
  /// </summary>
  /// <remarks>
  /// This function only works with console application.
  /// </remarks>
  /// <returns>Returns the code of the key pressed.</returns>
  int WaitKeyPress();

  /// <summary>
  /// Saves the current cursor position on a stack.
  /// </summary>
  void PushCursorPos();

  /// <summary>
  /// Restores the cursor position from a stack.
  /// </summary>
  void PopCursorPos();

  /// <summary>
  /// Returns the current cursor spinning sprite based on the requested refresh rate.
  /// </summary>
  /// <param name="refresh_rate">The duration time in seconds of a single sprite.</param>
  /// <returns>Returns the current cursor spinning sprite based on the requested refresh rate.</returns>
  char GetAnimationSprite(double refresh_rate);

  /// <summary>
  /// Prints a spinning cursor over time.
  /// </summary>
  /// <remarks>
  /// When the function exists, the cursor position is located over the spinning character.
  /// If user is done with the running task, the cursor must be erased by printing a space character over it.
  /// </remarks>
  void PrintAnimationCursor();

  /// <summary>Enumaration for all console text colors.</summary>
  enum TextColor {
    BLACK = 0,
    WHITE,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    GRAY,
    DARK_BLUE,
    DARK_GREEN,
    DARK_CYAN,
    DARK_RED,
    DARK_MAGENTA,
    DARK_YELLOW,
    DARK_GRAY,
  };

  static const int NUM_TEXT_COLOR = 16;

  /// <summary>
  /// Returns the name of the given text color.
  /// </summary>
  /// <remarks>The called does not have the ownsership of the returned value.</remarks>
  /// <returns>Returns the name of the given text color.</returns>
  const char * GetTextColorName(const TextColor & color);

  /// <summary>
  /// Set a new foreground and background text color.
  /// </summary>
  /// <param name="foreground">The text foreground color.</param>
  /// <param name="background">The text background color.</param>
  void SetTextColor(const TextColor & foreground, const TextColor & background);

  /// <summary>
  /// Get the current foreground and background text color.
  /// </summary>
  /// <param name="foreground">The output text foreground color.</param>
  /// <param name="background">The output text background color.</param>
  /// <remarks>
  /// On Windows, a GUI is always available (as the mane implies).
  /// On Linux, a GUI is optional. a desktop GUI is also not available if the application was launched from ssh.
  /// </remarks>
  void GetTextColor(TextColor & foreground, TextColor & background);

  /// <summary>
  /// Set default foreground and background text colors.
  /// </summary>
  void SetDefaultTextColor();

  /// <summary>
  /// Defines if the current running environment support a desktop GUI.
  /// </summary>
  /// <remarks>
  /// On Windows, a GUI is always available (as the mane implies).
  /// On Linux, a GUI is optional. a desktop GUI is also not available if the application was launched from ssh.
  /// </remarks>
  /// <returns>Returns true if a desktop gui is available. Returns false otherwise.</returns>
  bool IsDesktopGuiAvailable();

  /// <summary>
  /// Defines if the current running program is launched from desktop GUI.
  /// </summary>
  /// <remarks>
  /// On Windows, a GUI is always available (as the mane implies).
  /// On Linux, a GUI is optional. A desktop GUI is also not available if the application was launched from ssh.
  /// </remarks>
  /// <returns>Returns true if a desktop gui launched the program. Returns false otherwise.</returns>
  bool IsRunFromDesktop();

  /// <summary>
  /// Defines if the current running program has created the console window.
  /// </summary>
  /// <returns>Returns true if the current running program has created the console window. Returns false otherwise.</returns>
  bool HasConsoleOwnership();

  ///<summary>ANSI color support functions</summary>
  namespace ansi
  {
    //https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
    //https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

    namespace FormatAttribute
    {
      enum Attr {
        DEFAULT = 0,
        BOLD = 1,
        DIM = 2,
        UNDERLINED = 3,
        BLINK = 5,
        REVERSE = 7,
        HIDDEN = 8
      };
      const char * ToString(const Attr & attr);
      static const int NUM_COLOR_ATTR = 7;
    } //namespace FormatAttribute

    namespace ForegroundColor
    {
      enum Color {
        DEFAULT = 39,
        BLACK = 30,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        LIGHT_GRAY,
        DARK_GRAY = 90,
        LIGHT_RED,
        LIGHT_GREEN,
        LIGHT_YELLOW,
        LIGHT_BLUE,
        LIGHT_MAGENTA,
        LIGHT_CYAN,
        WHITE,
      };
      const char * ToString(const Color & color);
      static const int NUM_FOREGROUND_COLOR = 17;
    } //namespace ForegroundColor

    namespace BackgroundColor
    {
      enum Color {
        DEFAULT = 49,
        BLACK = 40,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        LIGHT_GRAY,
        DARK_GRAY = 100,
        LIGHT_RED,
        LIGHT_GREEN,
        LIGHT_YELLOW,
        LIGHT_BLUE,
        LIGHT_MAGENTA,
        LIGHT_CYAN,
        WHITE,
      };
      const char * ToString(const Color & color);
      static const int NUM_BACKGROUND_COLOR = 17;
    } //namespace BackgroundColor
  } //namespace ansi

} //namespace console
} //namespace ra

#endif //RA_CONSOLE_H
