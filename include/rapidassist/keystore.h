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

#ifndef RA_KEYSTORE_H
#define RA_KEYSTORE_H

#include <string>
#include <map>

namespace ra
{
  namespace keystore
  {

    class IKeyStore
    {
    public:

      virtual bool load(const std::string & path) = 0;
      virtual bool save(const std::string & path) = 0;
      virtual bool clear() = 0;

      virtual bool hasKey(const std::string & key) const = 0;
      virtual bool deleteKey(const std::string & key) = 0;
      virtual bool getValue(const std::string & key, std::string & value) const = 0;
      virtual bool setValue(const std::string & key, const std::string & value) = 0;
    };
    
    class PropertiesFileKeyStore : public virtual IKeyStore
    {
    public:
      PropertiesFileKeyStore();
      virtual ~PropertiesFileKeyStore();

      //IKeyStore interface
      virtual bool load(const std::string & file_path);
      virtual bool save(const std::string & file_path);
      virtual bool clear();
      virtual bool hasKey(const std::string & key) const;
      virtual bool deleteKey(const std::string & key);
      virtual bool getValue(const std::string & key, std::string & value) const;
      virtual bool setValue(const std::string & key, const std::string & value);

    private:
      typedef std::map<std::string /*keyname*/, std::string /*value*/> PropertyMap;
      PropertyMap mProperties;
    };

#ifdef _WIN32    
    class Win32RegistryKeyStore : public virtual IKeyStore
    {
    public:
      Win32RegistryKeyStore();
      virtual ~Win32RegistryKeyStore();

      //IKeyStore interface
      virtual bool load(const std::string & registry_path);
      virtual bool save(const std::string & registry_path);
      virtual bool clear();
      virtual bool hasKey(const std::string & key) const;
      virtual bool deleteKey(const std::string & key);
      virtual bool getValue(const std::string & key, std::string & value) const;
      virtual bool setValue(const std::string & key, const std::string & value);

    private:
      std::string mBaseRegistryPath;
    };
#endif //_WIN32

  } //namespace keystore
} //namespace ra

#endif //RA_KEYSTORE_H
