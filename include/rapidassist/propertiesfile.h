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

#include <string>
#include <map>

namespace ra { namespace filesystem {

  class PropertiesFile {
  public:
    PropertiesFile();
    virtual ~PropertiesFile();

    virtual bool load(const std::string & file_path);
    virtual bool save(const std::string & file_path);

    virtual bool clear();
    virtual bool hasKey(const std::string & key) const;
    virtual bool deleteKey(const std::string & key);
    virtual bool getValue(const std::string & key, std::string & value) const;
    virtual bool setValue(const std::string & key, const std::string & value);

  private:
    typedef std::map<std::string /*keyname*/, std::string /*value*/> PropertyMap;
    PropertyMap properties_;
  };

} //namespace filesystem
} //namespace ra

#endif //RA_PROPERTIESFILE_H
