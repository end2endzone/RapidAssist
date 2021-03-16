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

#include "rapidassist/filesystem_utf8.h"
#include "rapidassist/testing.h"
#include "rapidassist/unicode.h"

#include <stdint.h>
#include <algorithm>    // std::min
#include <string.h>

namespace ra { namespace filesystem {
  std::string ProcessCppEscapeCharacter(const std::string & value) {
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
      bool is_escaped_character = false;
      for (size_t j = 0; is_escaped_character == false && j < num_escape_sequences; j++) {
        const EscapeSequence & seq = sequences[j];
        size_t seq_value_length = strlen(seq.value);
        if (strncmp(c, seq.value, seq_value_length) == 0) {
          //this is an escape sequence
          is_escaped_character = true;

          //add the escaped sequence instead of the character
          text.append(1, seq.replacement);
          i += (strlen(seq.value) - 1); //-1 to cancel this loop's increment
        }
      }

      if (!is_escaped_character) {
        text.append(1, *c);
      }
    }

    return text;
  }

  size_t FindFirstUnescapedCharacter(const std::string & value, char c) {
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

  bool PropertiesFile::LoadUtf8(const std::string & file_path) {
    if (file_path.empty())
      return false;
 
    if (!ra::filesystem::FileExistsUtf8(file_path.c_str()))
      return false;
 
    Clear();
 
    // TODO: process file open here
    ra::strings::StringVector lines;
    bool loaded = ra::filesystem::ReadTextFileUtf8(file_path.c_str(), lines);
    if (!loaded)
      return false;
 
    loaded = Load(lines);
    return loaded;
  }

  bool PropertiesFile::Load(const std::string & file_path) {
    if (file_path.empty())
      return false;

    if (!ra::filesystem::FileExists(file_path.c_str()))
      return false;

    Clear();

    // TODO: process file open here
    ra::strings::StringVector lines;
    bool loaded = ra::filesystem::ReadTextFile(file_path.c_str(), lines);
    if (!loaded)
      return false;

    loaded = Load(lines);
    return loaded;
  }

  bool PropertiesFile::Load(const ra::strings::StringVector & lines) {
    std::string multiline_key;
    std::string multiline_value; //accumulator for multiline values

    for (size_t i = 0; i < lines.size(); i++) {
      std::string line = lines[i];
      line = ra::strings::Trim(line, ' ');
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
        size_t       equal_pos = FindFirstUnescapedCharacter(line, '='); //for format "key=value"
        size_t       colon_pos = FindFirstUnescapedCharacter(line, ':'); //for format "key:value"
        size_t       space_pos = FindFirstUnescapedCharacter(line, ' '); //for format "key value"
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
        key = ra::strings::TrimRight(key, ' ');
        value = ra::strings::TrimLeft(value, ' ');
      }

      key = ProcessCppEscapeCharacter(key);
      value = ProcessCppEscapeCharacter(value);

      //process properties specific espcape characters on the key
      ra::strings::Replace(key, "\\ ", " ");
      ra::strings::Replace(key, "\\:", ":");
      ra::strings::Replace(key, "\\=", "=");

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
          this->SetValue(multiline_key, multiline_value);
          multiline_key = "";
          multiline_value = "";
        }
        else {
          //single line key value
          this->SetValue(key, value);
        }
      }

      int a = 0;
    }

    return true;
  }

  bool PropertiesFile::SaveUtf8(const std::string & file_path) {
#ifdef _WIN32 // UTF-8
    const std::wstring pathW = ra::unicode::Utf8ToUnicode(file_path);
    FILE * f = _wfopen(pathW.c_str(), L"w");
#elif defined(__linux__) || defined(__APPLE__)
    FILE * f = fopen(file_path.c_str(), "w");
#endif // UTF-8

    if (!f)
      return false;
 
    bool saved = Save(f);
    return saved;
  }
 
  bool PropertiesFile::Save(const std::string & file_path) {
    FILE * f = fopen(file_path.c_str(), "w");
    if (!f)
      return false;
 
    bool saved = Save(f);
    return saved;
  }
 
  bool PropertiesFile::Save(FILE * f) {
    //for each properties
    for (PropertyMap::const_iterator propertyIt = properties_.begin(); propertyIt != properties_.end(); propertyIt++) {
      std::string key = propertyIt->first;
      const std::string & value = propertyIt->second;

      //remove key split characters
      ra::strings::Replace(key, " ", "\\ ");
      ra::strings::Replace(key, "=", "\\=");
      ra::strings::Replace(key, ":", "\\:");

      fprintf(f, "%s = %s\n", key.c_str(), value.c_str());
    }

    fclose(f);
    return true;
  }

  bool PropertiesFile::Clear() {
    properties_.clear();
    return true;
  }

  bool PropertiesFile::HasKey(const std::string & key) const {
    PropertyMap::const_iterator it = properties_.find(key);
    bool found = (it != properties_.end());
    return found;
  }

  bool PropertiesFile::DeleteKey(const std::string & key) {
    PropertyMap::iterator it = properties_.find(key);
    bool found = (it != properties_.end());
    if (!found)
      return true; //nothing to do

    properties_.erase(it);
    return true;
  }

  bool PropertiesFile::GetValue(const std::string & key, std::string & value) const {
    PropertyMap::const_iterator it = properties_.find(key);
    bool found = (it != properties_.end());
    if (!found)
      return false;

    value = it->second;
    return true;
  }

  bool PropertiesFile::SetValue(const std::string & key, const std::string & value) {
    //overwrite previous property
    properties_[key] = value;

    return true;
  }

} //namespace filesystem
} //namespace ra
