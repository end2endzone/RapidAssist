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

#ifndef RA_GENERICS_H
#define RA_GENERICS_H

#include <stdint.h>

namespace ra
{
  namespace generics
  {

    /// <summary>
    /// Swap two different variables.
    /// </summary>
    /// <param name="a">The first value.</param>
    /// <param name="b">The second value.</param>
    template <typename T> void swap(T & a, T & b)
    {
      T tmp;
      tmp = a;
      a = b;
      b = tmp;
    }

    /// <summary>
    /// Limit the given value between an acceptable range.
    /// </summary>
    /// <param name="low">The minimum allowed value.</param>
    /// <param name="high">The maximum allowed value.</param>
    /// <returns>Returns low if the given value is lower than low. Returns high if the given value is higher than high. Returns value otherwise.</returns>
    template <typename T> T constrain(const T & value, const T & low, const T & high)
    {
      return (value < low ? low : (value > high ? high : value) );
    }

    /// <summary>
    /// Returns the minimum value bewteen two given values.
    /// </summary>
    /// <param name="a">The first value.</param>
    /// <param name="b">The second value.</param>
    /// <returns>Returns the minimum value bewteen two given values.</returns>
    template <typename T> const T & minimum(const T & a, const T & b)
    {
      return (a < b ? a : b);
    }

    /// <summary>
    /// Returns the maximum value bewteen two given values.
    /// </summary>
    /// <param name="a">The first value.</param>
    /// <param name="b">The second value.</param>
    /// <returns>Returns the maximum value bewteen two given values.</returns>
    template <typename T> const T & maximum(const T & a, const T & b)
    {
      return (a > b ? a : b);
    }

    /// <summary>
    /// Maps the given value with a given input range to an output value that have a different output range.
    /// </summary>
    /// <param name="x">The input value.</param>
    /// <param name="in_min">The minimum value of the input value.</param>
    /// <param name="in_max">The maximum value of the input value.</param>
    /// <param name="out_min">The minimum value of the output value.</param>
    /// <param name="out_max">The maximum value of the output value.</param>
    /// <returns>Returns the output value.</returns>
    template <typename T> T map(const T & x, const T & in_min, const T & in_max, const T & out_min, const T & out_max)
    {
      T value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      return value;
    }

  } //namespace generics
} //namespace ra

#endif //RA_GENERICS_H