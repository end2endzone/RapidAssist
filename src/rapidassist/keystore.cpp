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

#include "rapidassist/keystore.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/gtesthelp.h"

#include <stdint.h>
#include <algorithm>    // std::min
#include <string.h>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN 1
#  endif
#  include <windows.h>
#  ifdef min
#    undef min
#  endif
#endif //_WIN32

namespace ra
{
  namespace keystore
  {

#ifdef _WIN32 //Win32RegistryKeyStore implementation
    struct HKEY_T
    {
      HKEY key;
      const char* name;
    };
    static HKEY_T supportedKeys[] = {
      {HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
      {HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
      {HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE"},
      {HKEY_USERS, "HKEY_USERS"},
      {HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG"},
    };
    static HKEY_T* findKeyInPath(const char* iPath)
    {
      //detecting the key within iPath
      int numSupportedKeys = sizeof(supportedKeys)/sizeof(HKEY_T);
      HKEY_T* rootKey = NULL;
      for(int i=0; i<numSupportedKeys && rootKey == NULL; i++)
      {
        HKEY_T* current = &supportedKeys[i];
        if (_strnicmp(iPath, current->name, strlen(current->name)) == 0)
        {
          rootKey = current;
        }
      }

      return rootKey;
    }
    static const char* getShortKeyPath(const char* iKeyPath)
    {
      HKEY_T* rootKey = findKeyInPath(iKeyPath);
      if (rootKey)
      {
        size_t keyPathIndex = strlen(rootKey->name)+1;
        return &iKeyPath[keyPathIndex];
      }
      return NULL;
    }

    bool getValueGeneric(const char * iKeyPath, const char * iValueName, DWORD & oType, std::string & oValue)
    {
      bool success = false;
      HKEY_T* rootKey = findKeyInPath(iKeyPath);
      
      if (rootKey)
      {
        HKEY keyHandle = NULL;
        const char* keyShortPath = getShortKeyPath(iKeyPath);

        if (RegOpenKeyEx(rootKey->key, keyShortPath, 0, KEY_QUERY_VALUE|KEY_WOW64_64KEY, &keyHandle) == ERROR_SUCCESS)
        {
          //Read value's size and type
          DWORD valueType = 0;
          DWORD valueSize = 0; //the size of the returned buffer in bytes. This size includes any terminating null character.
          if (RegQueryValueEx( keyHandle, iValueName, NULL, &valueType, NULL, &valueSize) == ERROR_SUCCESS)
          {
            DWORD length = valueSize - 1;

            //allocate space for value
            if (valueSize > 0 && oValue.assign(length, 0).size())
            {
              //Read the actual data of the value
              RegQueryValueEx( keyHandle, iValueName, NULL, &oType, (LPBYTE)oValue.c_str(), &valueSize);
              success = (length == oValue.size());
            }

            success = true;
          }

          RegCloseKey(keyHandle);
        }
      }

      return success;
    }

    bool getRegistryValues(const char * iKeyPath, ra::strings::StringVector & values)
    {
      values.clear();

      bool success = false;
      HKEY_T* rootKey = findKeyInPath(iKeyPath);
      
      if (rootKey)
      {
        HKEY keyHandle = NULL;
        const char* keyShortPath = getShortKeyPath(iKeyPath);

        if (RegOpenKeyEx(rootKey->key, keyShortPath, 0, KEY_QUERY_VALUE|KEY_WOW64_64KEY, &keyHandle) == ERROR_SUCCESS)
        {
          //https://docs.microsoft.com/en-us/windows/desktop/sysinfo/enumerating-registry-subkeys
          #define MAX_KEY_LENGTH 255
          #define MAX_VALUE_NAME 16383

          //TCHAR    achKey[MAX_KEY_LENGTH];  // buffer for subkey name
          //DWORD    cbName;                  // size of name string 
          TCHAR    achClass[MAX_PATH] = {0};  // buffer for class name 
          DWORD    cchClassName = MAX_PATH;   // size of class string 
          DWORD    cSubKeys=0;                // number of subkeys 
          DWORD    cbMaxSubKey;               // longest subkey size 
          DWORD    cchMaxClass;               // longest class string 
          DWORD    cValues;                   // number of values for key 
          DWORD    cchMaxValue;               // longest value name 
          DWORD    cbMaxValueData;            // longest value data 
          DWORD    cbSecurityDescriptor;      // size of security descriptor 
          FILETIME ftLastWriteTime;           // last write time 

          DWORD i, retCode; 

          TCHAR  achValue[MAX_VALUE_NAME]; 
          DWORD cchValue = MAX_VALUE_NAME; 

          // Get the class name and the value count. 
          retCode = RegQueryInfoKey(
            keyHandle,               // key handle 
            achClass,                // buffer for class name 
            &cchClassName,           // size of class string 
            NULL,                    // reserved 
            &cSubKeys,               // number of subkeys 
            &cbMaxSubKey,            // longest subkey size 
            &cchMaxClass,            // longest class string 
            &cValues,                // number of values for this key 
            &cchMaxValue,            // longest value name 
            &cbMaxValueData,         // longest value data 
            &cbSecurityDescriptor,   // security descriptor 
            &ftLastWriteTime);       // last write time 

          //// Enumerate the subkeys, until RegEnumKeyEx fails.
          //if (cSubKeys)
          //{
          //  for (i=0; i<cSubKeys; i++) 
          //  { 
          //    cbName = MAX_KEY_LENGTH;
          //    retCode = RegEnumKeyEx(keyHandle, i,
          //      achKey, 
          //      &cbName, 
          //      NULL, 
          //      NULL, 
          //      NULL, 
          //      &ftLastWriteTime); 
          //    if (retCode == ERROR_SUCCESS) 
          //    {
          //      //TODO: handle the subkey
          //    }
          //  }
          //} 

          // Enumerate the key values. 
          if (cValues) 
          {
            for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
            { 
              cchValue = MAX_VALUE_NAME; 
              achValue[0] = '\0'; 
              retCode = RegEnumValue(keyHandle, i, 
                achValue, 
                &cchValue, 
                NULL, 
                NULL,
                NULL,
                NULL);

              if (retCode == ERROR_SUCCESS ) 
              {
                if (cchValue == 0)
                  continue; //ignore default key value
                
                values.push_back(achValue);
              } 
            }
          }

          RegCloseKey(keyHandle);
          success = true;
        }
      }

      return success;
    }

    Win32RegistryKeyStore::Win32RegistryKeyStore()
    {
    }

    Win32RegistryKeyStore::~Win32RegistryKeyStore()
    {
    }

    //IKeyStore interface
    bool Win32RegistryKeyStore::load(const std::string & registry_path)
    {
      if (registry_path.empty())
        return false;

      // n/a window's registry does not requires pre-loading

      mBaseRegistryPath = registry_path;
      return true;
    }

    bool Win32RegistryKeyStore::save(const std::string & registry_path)
    {
      if (mBaseRegistryPath.empty() || registry_path.empty())
        return false; //not configured

      if (mBaseRegistryPath != registry_path)
        return false; //must use same loading path

      // n/a window's registry does not requires saving
      return true;
    }

    bool Win32RegistryKeyStore::clear()
    {
      if (mBaseRegistryPath.empty())
        return false; //not configured

      //find existing keys
      ra::strings::StringVector keys;
      bool find_ok = getRegistryValues(mBaseRegistryPath.c_str(), keys);
      if (!find_ok)
        return false;

      //delete each key
      for(size_t i=0; i<keys.size(); i++)
      {
        const std::string & key = keys[i];
        bool deleted = this->deleteKey(key);
        if (!deleted)
          return false;
      }

      return true;
    }

    bool Win32RegistryKeyStore::hasKey(const std::string & key) const
    {
      DWORD wKeyType = 0;
      std::string wBuffer;
      bool success = getValueGeneric(mBaseRegistryPath.c_str(), key.c_str(), wKeyType, wBuffer);
      return success;
    }

    bool Win32RegistryKeyStore::deleteKey(const std::string & key)
    {
      bool result = false;

      HKEY_T* rootKey = findKeyInPath(mBaseRegistryPath.c_str());
      if (rootKey)
      {
        const char* keyShortPath = getShortKeyPath(mBaseRegistryPath.c_str());

        HKEY keyHandle = NULL;
        if ( !RegOpenKeyEx(rootKey->key, keyShortPath, 0, KEY_SET_VALUE|KEY_WOW64_64KEY, &keyHandle))
        {
          LSTATUS status = RegDeleteValueA(keyHandle, key.c_str());
          if (status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND)
          {
            result = true;
          }
        }
        RegCloseKey(keyHandle);
      }

      return result;
    }

    bool Win32RegistryKeyStore::getValue(const std::string & key, std::string & value) const
    {
      DWORD wKeyType = 0;
      std::string wBuffer;
      bool success = getValueGeneric(mBaseRegistryPath.c_str(), key.c_str(), wKeyType, wBuffer);
      if (!success)
        return false;

      //handle expand env var
      if (wKeyType == REG_EXPAND_SZ)
      {
        static const DWORD BUFFER_SIZE = 10240;
        TCHAR temp[BUFFER_SIZE];
        ExpandEnvironmentStrings(wBuffer.c_str(), temp, BUFFER_SIZE);
        wBuffer = temp;
      }

      switch(wKeyType)
      {
      case REG_SZ:
      case REG_EXPAND_SZ:
        value = wBuffer;
        break;
      case REG_DWORD:
        {
          const uint32_t * tmp = reinterpret_cast<const uint32_t *>(wBuffer.c_str());
          std::string tmp_str = ra::strings::toString(*tmp);
          value = tmp_str;
        }
        break;
      case REG_QWORD:
        {
          const uint64_t * tmp = reinterpret_cast<const uint64_t *>(wBuffer.c_str());
          std::string tmp_str = ra::strings::toString(*tmp);
          value = tmp_str;
        }
        break;
      default:
        return false; //unknown type
      };

      return true;
    }

    bool Win32RegistryKeyStore::setValue(const std::string & key, const std::string & value)
    {
      if (mBaseRegistryPath.empty() || key.empty())
        return false; //not configured

      bool result = false;

      HKEY_T* rootKey = findKeyInPath(mBaseRegistryPath.c_str());
      if (rootKey)
      {
        const char* keyShortPath = getShortKeyPath(mBaseRegistryPath.c_str());

        HKEY keyHandle;
        if ( !RegCreateKey(rootKey->key, keyShortPath, &keyHandle))
        {
          DWORD size = (DWORD)value.size() + 1;
          if ( !RegSetValueEx(keyHandle, key.c_str(), 0, REG_SZ, (LPBYTE)value.c_str(), size))
          {
            result = true;
          }
          RegCloseKey(keyHandle);
        }
      }
      return result;
    }
#endif // Win32RegistryKeyStore

#if 1 //PropertiesFileKeyStore
    std::string processCppEscapeCharacter(const std::string & value)
    {
      //https://en.cppreference.com/w/cpp/language/escape
      //https://en.wikipedia.org/wiki/Escape_sequences_in_C
      struct ESPACESEQUENCE
      {
        const char * value;
        char replacement;
      };
      static const ESPACESEQUENCE sequences[] = {
        {"\\a", '\a'},  //beep
        {"\\b", '\b'},  //backspace
        {"\\e", 0x1b},  //esc
        {"\\f", '\f'},  //formfeed
        {"\\n", '\n'},  //newline
        {"\\r", '\r'},  //carriage return
        {"\\t", '\t'},  //tab
        {"\\v", '\v'},  //vertical tab
        {"\\\\", '\\'}, //backslash
        {"\\\'", '\''}, //single quotation mark
        {"\\\"", '\"'}, //double quotation mark
        {"\\?", '?'},   //question mark
        /*Unsupported escape sequences:
          \nnn
          \xhh
          \uhhhh
          \uhhhhhhhh
        */
      };
      static const size_t num_escape_sequences = sizeof(sequences)/sizeof(sequences[0]);
      
      std::string text;
      for(size_t i=0; i<value.size(); i++)
      {
        const char * c = &value[i];

        //for each escape sequence
        bool isEscapedCharacter = false;
        for(size_t j=0; isEscapedCharacter == false && j<num_escape_sequences; j++)
        {
          const ESPACESEQUENCE & seq = sequences[j];
          size_t seq_value_length = strlen(seq.value);
          if ( strncmp(c, seq.value, seq_value_length) == 0 )
          {
            //this is an escape sequence
            isEscapedCharacter = true;

            //add the escaped sequence instead of the character
            text.append(1, seq.replacement);
            i += (strlen(seq.value) - 1); //-1 to cancel this loop's increment
          }
        }

        if (!isEscapedCharacter)
        {
          text.append(1, *c);
        }
      }

      return text;
    }

    size_t findFirstUnescapedCharacter(const std::string & value, char c)
    {
      for(size_t i=0; i<value.size(); i++)
      {
        char tmp = value[i];
        if (tmp == '\\')
        {
          i++; //also skip next escaped character
        }
        else if (tmp == c)
        {
          return i; //that's the character we are looking for
        }
      }

      return std::string::npos;
    }

    PropertiesFileKeyStore::PropertiesFileKeyStore()
    {
    }

    PropertiesFileKeyStore::~PropertiesFileKeyStore()
    {
    }

    //IKeyStore interface
    bool PropertiesFileKeyStore::load(const std::string & file_path)
    {
      if (file_path.empty())
        return false;

      if (!ra::filesystem::fileExists(file_path.c_str()))
        return false;

      clear();

      // TODO: process file open here
      ra::strings::StringVector lines;
      bool loaded = ra::gtesthelp::getTextFileContent(file_path.c_str(), lines);
      if (!loaded)
        return false;

      std::string multiline_key;
      std::string multiline_value; //accumulator for multiline values

      for(size_t i=0; i<lines.size(); i++)
      {
        std::string line = lines[i];
        line = ra::strings::trim(line, ' ');
        if (line.empty())
          continue;
        else if (line[0] == '#' || line[0] == '!')
          continue; //this is a comment line
        
        //define placeholder for key and value
        std::string key;
        std::string value;

        //if we parsed a multiline key/value on previous line
        if (!multiline_key.empty())
        {
          key = "";
          value = line;
        }
        else
        {
          //search for the first '=', ':' or unescaped ' ' character
          size_t split_pos = std::string::npos;
          size_t space_equal_pos = line.find(" = ");                       //for format "key = value"
          size_t space_colon_pos = line.find(" : ");                       //for format "key : value"
          size_t       equal_pos = findFirstUnescapedCharacter(line, '='); //for format "key=value"
          size_t       colon_pos = findFirstUnescapedCharacter(line, ':'); //for format "key:value"
          size_t       space_pos = findFirstUnescapedCharacter(line, ' '); //for format "key value"
          split_pos = std::min(split_pos, space_equal_pos);
          split_pos = std::min(split_pos, space_colon_pos);
          split_pos = std::min(split_pos,       equal_pos);
          split_pos = std::min(split_pos,       colon_pos);
          split_pos = std::min(split_pos,       space_pos);
          if (split_pos != std::string::npos)
          {
            //compute split length
            size_t split_length = 1; //by default
            if (split_pos == space_equal_pos ||
                split_pos == space_colon_pos)
            {
              split_length = 3;
            }

            //split as "key" and "value"
            key   = line.substr(0, split_pos);
            size_t value_length = line.size() - split_pos - split_length;
            value = line.substr(split_pos+split_length, value_length);
          }
          else
            continue; //no key-value split character in string

          //trim both to support the format "var=value" and "var = value"
          key = ra::strings::trimRight(key, ' ');
          value = ra::strings::trimLeft(value, ' ');
        }

        key   = processCppEscapeCharacter(key);
        value = processCppEscapeCharacter(value);

        //process properties specific espcape characters on the key
        ra::strings::replace(key, "\\ ", " ");
        ra::strings::replace(key, "\\:", ":");
        ra::strings::replace(key, "\\=", "=");

        //is value continued on next line
        if (!value.empty() && value[value.size()-1] == '\\')
        {
          //this key is a multiline value

          //remove last \ character
          value.erase(value.size()-1, 1);

          //save the current key & value for the next line
          if (!key.empty())
            multiline_key = key;
          multiline_value += value;
        }
        else
        {
          //save the loaded key/value pair

          //are we dealing with a multiline value?
          if (!multiline_key.empty())
          {
            multiline_value += value;
            this->setValue(multiline_key, multiline_value);
            multiline_key   = "";
            multiline_value = "";
          }
          else
          {
            //single line key value
            this->setValue(key, value);
          }
        }

        int a = 0;
      }

      return true;
    }

    bool PropertiesFileKeyStore::save(const std::string & file_path)
    {
      FILE * f = fopen(file_path.c_str(), "w");
      if (!f)
        return false;

      //for each properties
      for (PropertyMap::const_iterator propertyIt = mProperties.begin(); propertyIt != mProperties.end(); propertyIt++)
      {
        std::string key   = propertyIt->first;
        const std::string & value = propertyIt->second;

        //remove key split characters
        ra::strings::replace(key, " ", "\\ ");
        ra::strings::replace(key, "=", "\\=");
        ra::strings::replace(key, ":", "\\:");

        fprintf(f, "%s = %s\n", key.c_str(), value.c_str());
      }

      fclose(f);
      return true;
    }

    bool PropertiesFileKeyStore::clear()
    {
      mProperties.clear();
      return true;
    }

    bool PropertiesFileKeyStore::hasKey(const std::string & key) const
    {
      PropertyMap::const_iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      return found;
    }

    bool PropertiesFileKeyStore::deleteKey(const std::string & key)
    {
      PropertyMap::iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      if (!found)
        return true; //nothing to do

      mProperties.erase(propertyIt);
      return true;
    }

    bool PropertiesFileKeyStore::getValue(const std::string & key, std::string & value) const
    {
      PropertyMap::const_iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      if (!found)
        return false;

      value = propertyIt->second;
      return true;
    }

    bool PropertiesFileKeyStore::setValue(const std::string & key, const std::string & value)
    {
      //overwrite previous property
      mProperties[key] = value;

      return true;
    }
#endif //PropertiesFileKeyStore

  } //namespace keystore
} //namespace ra
