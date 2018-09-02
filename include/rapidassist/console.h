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

namespace ra
{
  namespace console
  {

    /// <summary>
    /// Returns the cursor position.
    /// </summary>
    /// <remarks>The cosole x and y coordinate system is 0 based</remarks>
    /// <param name="x">The x coordinate of the cursor.</param>
    /// <param name="y">The y coordinate of the cursor.</param>
    void getCursorPos(int & x, int & y);

    /// <summary>
    /// Sets the cursor position.
    /// </summary>
    /// <remarks>The cosole x and y coordinate system is 0 based</remarks>
    /// <param name="x">The x coordinate of the cursor.</param>
    /// <param name="y">The y coordinate of the cursor.</param>
    void setCursorPos(const int & x, const int & y);

    /// <summary>
    /// Clears the screen.
    /// </summary>
    void clearScreen();

    /// <summary>
    /// Saves the current cursor position on a stack.
    /// </summary>
    void pushCursorPos();

    /// <summary>
    /// Restores the cursor position from a stack.
    /// </summary>
    void popCursorPos();

    /// <summary>
    /// Returns the current cursor spinning sprite based on the requested refresh rate.
    /// </summary>
    /// <param name="iRefreshRate">The duration time in seconds of a single sprite.</param>
    /// <returns>Returns the current cursor spinning sprite based on the requested refresh rate.</returns>
    char getAnimationSprite(double iRefreshRate);

    /// <summary>
    /// Prints a spinning cursor over time.
    /// </summary>
    /// <remarks>
    /// When the function exists, the cursor position is located over the spinning character.
    /// If user is done with the running task, the cursor must be erased by printing a space character over it.
    /// </remarks>
    void printAnimationCursor();

    /// <summary>Enumaration for all console text colors.</summary>
    enum TextColor
    {
      Black = 0,
      White,
      Blue,
      Green,
      Cyan,
      Red,
      Magenta,
      Yellow,
      Gray,
      DarkBlue,
      DarkGreen,
      DarkCyan,
      DarkRed,
      DarkMagenta,
      DarkYellow,
      DarkGray,
    };

    static const int NUM_TEXT_COLOR = 16;

    /// <summary>
    /// Returns the name of the given text color.
    /// </summary>
    /// <remarks>The called does not have the ownsership of the returned value.</remarks>
    /// <returns>Returns the name of the given text color.</returns>
    const char * getTextColorName(const TextColor & color);

    /// <summary>
    /// Set a new foreground and background text color.
    /// </summary>
    /// <param name="iForeground">The text foreground color.</param>
    /// <param name="iBackground">The text background color.</param>
    void setTextColor(const TextColor & iForeground, const TextColor & iBackground);

    /// <summary>
    /// Get the current foreground and background text color.
    /// </summary>
    /// <param name="oForeground">The text foreground color.</param>
    /// <param name="oBackground">The text background color.</param>
    void getTextColor(TextColor & oForeground, TextColor & oBackground);

    /// <summary>
    /// Set default foreground and background text colors.
    /// </summary>
    void setDefaultTextColor();

  } //namespace console
} //namespace ra

#endif //RA_CONSOLE_H
