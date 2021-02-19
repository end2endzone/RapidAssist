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
#include <vector>
#include <list>
#include <set>
#include <algorithm>

#include "rapidassist/config.h"

namespace ra { namespace generics {

  static const size_t INVALID_INDEX = (size_t)-1;

  /// <summary>
  /// Swap two different variables.
  /// </summary>
  /// <param name="a">The first value.</param>
  /// <param name="b">The second value.</param>
  template <typename T> inline void Swap(T & a, T & b) {
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
  template <typename T> inline T Constrain(const T & value, const T & low, const T & high) {
    return (value < low ? low : (value > high ? high : value));
  }

  /// <summary>
  /// Returns the minimum value bewteen two given values.
  /// </summary>
  /// <param name="a">The first value.</param>
  /// <param name="b">The second value.</param>
  /// <returns>Returns the minimum value bewteen two given values.</returns>
  template <typename T> inline const T & Minimum(const T & a, const T & b) {
    return (a < b ? a : b);
  }

  /// <summary>
  /// Returns the maximum value bewteen two given values.
  /// </summary>
  /// <param name="a">The first value.</param>
  /// <param name="b">The second value.</param>
  /// <returns>Returns the maximum value bewteen two given values.</returns>
  template <typename T> inline const T & Maximum(const T & a, const T & b) {
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
  template <typename T> inline T Map(const T & x, const T & in_min, const T & in_max, const T & out_min, const T & out_max) {
    T value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return value;
  }

  /// <summary>
  /// Returns true if the two given number are near each other given an epsilon value.
  /// </summary>
  /// <param name="value1">The first value.</param>
  /// <param name="value2">The second value.</param>
  /// <param name="epsilon">The epsilon value (tolerance).</param>
  /// <returns>Returns true if the two given number are near each other given an epsilon value.</returns>
  template <typename T> inline bool Near(const T & value1, const T & value2, const T & epsilon) {
    T value1_min = value1 - epsilon;
    T value1_max = value1 + epsilon;
    if (value1_min <= value2 && value2 <= value1_max)
      return true;
    else
      return false;
  }

  /// <summary>
  /// Copy the memory representation of a given type to another type.
  /// </summary>
  /// <param name="value">The value to convert as another type.</param>
  /// <returns>Returns the memory representation of a given type into another type.</returns>
  template <typename T, typename S> inline T ReadAs(const S & value) {
    const void * sPtr = &value;
    const T * tPtr = static_cast<const T *>(sPtr);
    const T & tValue = (*tPtr);
    return tValue;
  }

  /// <summary>
  /// Reverse the elements of the given std::vector.
  /// </summary>
  /// <param name="values">The list of values to flip upside down.</param>
  /// <returns>Returns copy of the list with the elements in the reserve order.</returns>
  template <typename T> inline void ReverseVector(const std::vector<T> & input, std::vector<T> & output) {
    output.clear();
    output.reserve(input.size());

    if (!input.empty()) {
      //proceed with the reverseing algorithm
      size_t num_elements = input.size();
      for (size_t i = num_elements - 1; i != ra::generics::INVALID_INDEX; i--) {
        const T & element = input[i];
        output.push_back(element);
      }
    }
  }

  /// <summary>
  /// Finds a value within a list of elements.
  /// </summary>
  /// <param name="values">The list of values to search.</param>
  /// <param name="value">The value that will be searched within the list.</param>
  /// <returns>Returns true if the value was found. Returns false otherwise.</returns>
  template <typename T> inline bool HasElement(const std::vector<T> & values, const T & value) {
    typename std::vector<T>::const_iterator it = std::find(values.begin(), values.end(), value);
    if (it != values.end())
      return true;
    return false;
  }

  /// <summary>
  /// Finds a value within a list of elements.
  /// </summary>
  /// <param name="values">The list of values to search.</param>
  /// <param name="value">The value that will be searched within the list.</param>
  /// <returns>Returns true if the value was found. Returns false otherwise.</returns>
  template <typename T> inline bool HasElement(const std::list<T> & values, const T & value) {
    typename std::vector<T>::iterator it = std::find(values.begin(), values.end(), value);
    if (it != values.end())
      return true;
    return false;
  }

  /// <summary>
  /// Finds the maximum value of the given list and return its index.
  /// </summary>
  /// <param name="values">The list of values to search for the value.</param>
  /// <returns>Returns the index of the maximum value. Returns ra::generics::INVALID_INDEX if not found.</returns>
  template <typename T> inline size_t FindMaxIndex(const std::vector<T> & values) {
    size_t num_items = values.size();
    if (num_items == 0)
      return ra::generics::INVALID_INDEX;
    else if (num_items == 1)
      return 0;
    else {
      size_t index = 0; //first value is the maximum by default
      T max_item = values[index];

      //loop through all other values
      for (size_t i = 1; i < values.size(); i++) {
        const T & item = values[i];
        if (item > max_item) {
          index = i;
          max_item = values[index];
        }
      }
      return index;
    }
  }

  /// <summary>
  /// Finds the minimum value of the given list and return its index.
  /// </summary>
  /// <param name="values">The list of values to search for the value.</param>
  /// <returns>Returns the index of the minimum value. Returns ra::generics::INVALID_INDEX if not found.</returns>
  template <typename T> inline size_t FindMinIndex(const std::vector<T> & values) {
    size_t num_items = values.size();
    if (num_items == 0)
      return ra::generics::INVALID_INDEX;
    else if (num_items == 1)
      return 0;
    else {
      size_t index = 0; //first value is the minimum by default
      T min_item = values[index];

      //loop through all other values
      for (size_t i = 1; i < values.size(); i++) {
        const T & item = values[i];
        if (item < min_item) {
          index = i;
          min_item = values[index];
        }
      }
      return index;
    }
  }

  /// <summary>
  /// Copy all the unique elements of the given list to another list.
  /// </summary>
  /// <param name="values">The list of values that must be made unique.</param>
  /// <returns>Returns a copy of the given list with all the element unique.</returns>
  template <typename T> inline std::vector<T> CopyUnique(const std::vector<T> & values) {
    std::vector<T> list_copy;
    for (size_t i = 0; i < values.size(); i++) {
      const T & item = values[i];
      bool exists = HasElement<T>(list_copy, item);
      if (!exists)
        list_copy.push_back(item);
    }
    return list_copy;
  }

  /// <summary>
  /// Removes duplicate items from the given list.
  /// </summary>
  /// <param name="values">The list of values that must be made unique.</param>
  /// <returns>Returns the number of elements that was removed.</returns>
  template <typename T> inline size_t MakeUnique(std::vector<T> & values) {
    //search duplicate values in the list.
    std::set<T> tmp_copy;
    std::vector<size_t> duplicates;
    for (size_t i = 0; i < values.size(); i++) {
      const T & item = values[i];

      //when a duplicate is found, keep its index value
      bool exists = (std::find(tmp_copy.begin(), tmp_copy.end(), item) != tmp_copy.end());
      if (exists) {
        //this element is a duplicate
        duplicates.push_back(i);
      }
      else {
        //remember this element
        tmp_copy.insert(item);
      }
    }

    //remove duplicates from the list
    if (!duplicates.empty()) {
      //elements must be removed in the reverse order
      size_t num_duplicates = duplicates.size();
      for (size_t i = num_duplicates - 1; i != ra::generics::INVALID_INDEX; i--) {
        const size_t & index = duplicates[i];
        values.erase(values.begin() + index);
      }
    }

    return duplicates.size();
  }

  /// <summary>
  /// Finds the maximum value of the given list.
  /// </summary>
  /// <param name="values">The list of values to search for the value.</param>
  /// <returns>Returns a pointer to the maximum value. Returns NULL if not found.</returns>
  template <typename T> inline const T * FindMaxValue(const std::vector<T> & values) {
    size_t num_items = values.size();
    if (num_items == 0)
      return NULL;
    else if (num_items == 1)
      return &values[0];
    else {
      size_t index = 0; //first value is the maximum by default
      const T * max_item = &values[index];

      //loop through all other values
      for (size_t i = 1; i < values.size(); i++) {
        const T & item = values[i];
        if (item > (*max_item)) {
          index = i;
          max_item = &values[index];
        }
      }
      return max_item;
    }
  }

  /// <summary>
  /// Finds the minimum value of the given list.
  /// </summary>
  /// <param name="values">The list of values to search for the value.</param>
  /// <returns>Returns a pointer to the minimum value. Returns NULL if not found.</returns>
  template <typename T> inline const T * FindMinValue(const std::vector<T> & values) {
    size_t num_items = values.size();
    if (num_items == 0)
      return NULL;
    else if (num_items == 1)
      return &values[0];
    else {
      size_t index = 0; //first value is the minimum by default
      const T * min_item = &values[index];

      //loop through all other values
      for (size_t i = 1; i < values.size(); i++) {
        const T & item = values[i];
        if (item < (*min_item)) {
          index = i;
          min_item = &values[index];
        }
      }
      return min_item;
    }
  }

  /// <summary>
  /// Finds all indice of the given value within an array of values.
  /// </summary>
  /// <param name="value">The value to search within the array.</param>
  /// <param name="array_">The array to search into.</param>
  /// <param name="size">The number of elements in the array.</param>
  /// <returns>Returns list of index where the searched value was found in the given array. Returns an empty list if the value is not found.</returns>
  template <typename T> inline std::vector<size_t> FindIndexOf(const T & value, const T * array_, const size_t & size) {
    std::vector<size_t> matches;

    for (size_t i = 0; i < size; i++) {
      const T & item = array_[i];
      if (item == value) {
        matches.push_back(i);
      }
    }

    return matches;
  }

  /// <summary>
  /// Finds the first occurange of a value within an array of values.
  /// </summary>
  /// <param name="value">The value to search within the array.</param>
  /// <param name="array_">The array to search into.</param>
  /// <param name="size">The number of elements in the array.</param>
  /// <returns>Returns the index where the values as found in the array. Returns ra::generics::INVALID_INDEX if the value is not found.</returns>
  template <typename T> inline size_t FindFirst(const T & value, const T * array_, const size_t & size) {
    for (size_t i = 0; i < size; i++) {
      const T & item = array_[i];
      if (item == value) {
        return i;
      }
    }

    return ra::generics::INVALID_INDEX;
  }

} //namespace generics
} //namespace ra

#endif //RA_GENERICS_H
