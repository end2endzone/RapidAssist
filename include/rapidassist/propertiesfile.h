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

#ifndef RA_PROPERTIESFILE_H
#define RA_PROPERTIESFILE_H

#include <map>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"

namespace ra { namespace filesystem {

  /// <summary>
  /// Utility class for reading and writing *.properties file.
  /// </summary>
  class PropertiesFile {
  public:
    /// <summary>
    /// Ctor for the PropertiesFile class.
    /// </summary>
    PropertiesFile();

    /// <summary>
    /// Dtor for the PropertiesFile class.
    /// </summary>
    virtual ~PropertiesFile();

    /// <summary>
    /// Load a properties file into memory.
    /// </summary>
    /// <param name="file_path">The path to properties file.</param>
    /// <returns>Returns true if the load operation is successful. Returns false otherwise.</returns>
    virtual bool Load(const std::string & file_path);

    /// <summary>
    /// Save the properties set in memory to file.
    /// </summary>
    /// <param name="file_path">The path to properties file.</param>
    /// <returns>Returns true if the save operation is successful. Returns false otherwise.</returns>
    virtual bool Save(const std::string & file_path);

    /// <summary>
    /// Load a properties file into memory.
    /// The file path must be specified in utf-8 format.
    /// </summary>
    /// <param name="file_path">The path to properties file in utf-8 format.</param>
    /// <returns>Returns true if the load operation is successful. Returns false otherwise.</returns>
    virtual bool LoadUtf8(const std::string & file_path);

    /// <summary>
    /// Save the properties set in memory to file.
    /// The file path must be specified in utf-8 format.
    /// </summary>
    /// <param name="file_path">The path to properties file in utf-8 format.</param>
    /// <returns>Returns true if the save operation is successful. Returns false otherwise.</returns>
    virtual bool SaveUtf8(const std::string & file_path);

    /// <summary>
    /// Erase all existing properties set in memory.
    /// </summary>
    /// <returns>Returns true if the operation is successful. Returns false otherwise.</returns>
    virtual bool Clear();

    /// <summary>
    /// Search for a specific property name.
    /// </summary>
    /// <param name="key">The name of the property.</param>
    /// <returns>Returns true if the property name is found. Returns false otherwise.</returns>
    virtual bool HasKey(const std::string & key) const;

    /// <summary>
    /// Delete a specific property name.
    /// </summary>
    /// <param name="key">The name of the property.</param>
    /// <returns>Returns true if the property was deleted or does not exists. Returns false otherwise.</returns>
    virtual bool DeleteKey(const std::string & key);

    /// <summary>
    /// Get the value of a specific property.
    /// </summary>
    /// <param name="key">The name of the property.</param>
    /// <param name="value">The output property value.</param>
    /// <returns>Returns true if the property name is found. Returns false otherwise.</returns>
    virtual bool GetValue(const std::string & key, std::string & value) const;

    /// <summary>
    /// Set or override the value of a specific property.
    /// </summary>
    /// <param name="key">The name of the property.</param>
    /// <param name="value">The new property value.</param>
    /// <returns>Returns true if the property was set. Returns false otherwise.</returns>
    virtual bool SetValue(const std::string & key, const std::string & value);

  private:
    /// <summary>
    /// Load a properties file into memory.
    /// </summary>
    /// <param name="lines">The content (an array of lines) of a properties file.</param>
    /// <returns>Returns true if the load operation is successful. Returns false otherwise.</returns>
    bool Load(const ra::strings::StringVector & lines);

    /// <summary>
    /// Save the properties set in memory to file.
    /// The given FILE* pointer is closed if the operation is successful.
    /// </summary>
    /// <param name="f">A valid FILE pointer.</param>
    /// <returns>Returns true if the save operation is successful. Returns false otherwise.</returns>
    bool Save(FILE * f);

  private:
    typedef std::map<std::string /*keyname*/, std::string /*value*/> PropertyMap;
    PropertyMap properties_;
  };

} //namespace filesystem
} //namespace ra

#endif //RA_PROPERTIESFILE_H
