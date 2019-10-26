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

#include "rapidassist/propertiesfile.h"

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/testing.h"

#include <stdint.h>
#include <algorithm>    // std::min
#include <string.h>

namespace ra
{
  namespace filesystem
  {

    std::string processCppEscapeCharacter(const std::string & value) {
      //https://en.cppreference.com/w/cpp/language/escape
      //https://en.wikipedia.org/wiki/Escape_sequences_in_C
      struct EscapeSequence {
        const char * value;
        char replacement;
      };
      static const EscapeSequence sequences[] = {
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
      static const size_t num_escape_sequences = sizeof(sequences) / sizeof(sequences[0]);

      std::string text;
      for (size_t i = 0; i < value.size(); i++) {
        const char * c = &value[i];

        //for each escape sequence
        bool isEscapedCharacter = false;
        for (size_t j = 0; isEscapedCharacter == false && j < num_escape_sequences; j++) {
          const EscapeSequence & seq = sequences[j];
          size_t seq_value_length = strlen(seq.value);
          if (strncmp(c, seq.value, seq_value_length) == 0) {
            //this is an escape sequence
            isEscapedCharacter = true;

            //add the escaped sequence instead of the character
            text.append(1, seq.replacement);
            i += (strlen(seq.value) - 1); //-1 to cancel this loop's increment
          }
        }

        if (!isEscapedCharacter) {
          text.append(1, *c);
        }
      }

      return text;
    }

    size_t findFirstUnescapedCharacter(const std::string & value, char c) {
      for (size_t i = 0; i < value.size(); i++) {
        char tmp = value[i];
        if (tmp == '\\') {
          i++; //also skip next escaped character
        }
        else if (tmp == c) {
          return i; //that's the character we are looking for
        }
      }

      return std::string::npos;
    }

    PropertiesFile::PropertiesFile() {
    }

    PropertiesFile::~PropertiesFile() {
    }

    bool PropertiesFile::load(const std::string & file_path) {
      if (file_path.empty())
        return false;

      if (!ra::filesystem::fileExists(file_path.c_str()))
        return false;

      clear();

      // TODO: process file open here
      ra::strings::StringVector lines;
      bool loaded = ra::testing::getTextFileContent(file_path.c_str(), lines);
      if (!loaded)
        return false;

      std::string multiline_key;
      std::string multiline_value; //accumulator for multiline values

      for (size_t i = 0; i < lines.size(); i++) {
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
        if (!multiline_key.empty()) {
          key = "";
          value = line;
        }
        else {
          //search for the first '=', ':' or unescaped ' ' character
          size_t split_pos = std::string::npos;
          size_t space_equal_pos = line.find(" = ");                       //for format "key = value"
          size_t space_colon_pos = line.find(" : ");                       //for format "key : value"
          size_t       equal_pos = findFirstUnescapedCharacter(line, '='); //for format "key=value"
          size_t       colon_pos = findFirstUnescapedCharacter(line, ':'); //for format "key:value"
          size_t       space_pos = findFirstUnescapedCharacter(line, ' '); //for format "key value"
          split_pos = std::min(split_pos, space_equal_pos);
          split_pos = std::min(split_pos, space_colon_pos);
          split_pos = std::min(split_pos, equal_pos);
          split_pos = std::min(split_pos, colon_pos);
          split_pos = std::min(split_pos, space_pos);
          if (split_pos != std::string::npos) {
            //compute split length
            size_t split_length = 1; //by default
            if (split_pos == space_equal_pos ||
              split_pos == space_colon_pos) {
              split_length = 3;
            }

            //split as "key" and "value"
            key = line.substr(0, split_pos);
            size_t value_length = line.size() - split_pos - split_length;
            value = line.substr(split_pos + split_length, value_length);
          }
          else
            continue; //no key-value split character in string

          //trim both to support the format "var=value" and "var = value"
          key = ra::strings::trimRight(key, ' ');
          value = ra::strings::trimLeft(value, ' ');
        }

        key = processCppEscapeCharacter(key);
        value = processCppEscapeCharacter(value);

        //process properties specific espcape characters on the key
        ra::strings::replace(key, "\\ ", " ");
        ra::strings::replace(key, "\\:", ":");
        ra::strings::replace(key, "\\=", "=");

        //is value continued on next line
        if (!value.empty() && value[value.size() - 1] == '\\') {
          //this key is a multiline value

          //remove last \ character
          value.erase(value.size() - 1, 1);

          //save the current key & value for the next line
          if (!key.empty())
            multiline_key = key;
          multiline_value += value;
        }
        else {
          //save the loaded key/value pair

          //are we dealing with a multiline value?
          if (!multiline_key.empty()) {
            multiline_value += value;
            this->setValue(multiline_key, multiline_value);
            multiline_key = "";
            multiline_value = "";
          }
          else {
            //single line key value
            this->setValue(key, value);
          }
        }

        int a = 0;
      }

      return true;
    }

    bool PropertiesFile::save(const std::string & file_path) {
      FILE * f = fopen(file_path.c_str(), "w");
      if (!f)
        return false;

      //for each properties
      for (PropertyMap::const_iterator propertyIt = mProperties.begin(); propertyIt != mProperties.end(); propertyIt++) {
        std::string key = propertyIt->first;
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

    bool PropertiesFile::clear() {
      mProperties.clear();
      return true;
    }

    bool PropertiesFile::hasKey(const std::string & key) const {
      PropertyMap::const_iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      return found;
    }

    bool PropertiesFile::deleteKey(const std::string & key) {
      PropertyMap::iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      if (!found)
        return true; //nothing to do

      mProperties.erase(propertyIt);
      return true;
    }

    bool PropertiesFile::getValue(const std::string & key, std::string & value) const {
      PropertyMap::const_iterator propertyIt = mProperties.find(key);
      bool found = (propertyIt != mProperties.end());
      if (!found)
        return false;

      value = propertyIt->second;
      return true;
    }

    bool PropertiesFile::setValue(const std::string & key, const std::string & value) {
      //overwrite previous property
      mProperties[key] = value;

      return true;
    }

  } //namespace filesystem
} //namespace ra
