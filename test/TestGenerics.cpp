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

#include "TestGenerics.h"
#include "rapidassist/generics.h"
#include "rapidassist/strings.h"

namespace ra { namespace generics { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestGenerics::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestGenerics::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testSwapInt) {
    int a = 5;
    int b = 9;
    ra::generics::Swap(a, b);

    ASSERT_EQ(5, b);
    ASSERT_EQ(9, a);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testSwapString) {
    std::string a = "abc";
    std::string b = "def";
    ra::generics::Swap(a, b);

    ASSERT_EQ("abc", b);
    ASSERT_EQ("def", a);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMinimum) {
    int a = 5;
    int b = 9;

    ASSERT_EQ(5, ra::generics::Minimum(a, b));
    ASSERT_EQ(5, ra::generics::Minimum(b, a));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMaximum) {
    int a = 5;
    int b = 9;

    ASSERT_EQ(9, ra::generics::Maximum(a, b));
    ASSERT_EQ(9, ra::generics::Maximum(b, a));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testConstrain) {
    int low = 10;
    int high = 20;

    ASSERT_EQ(low, ra::generics::Constrain(5, low, high));
    ASSERT_EQ(high, ra::generics::Constrain(999, low, high));
    ASSERT_EQ(15, ra::generics::Constrain(15, low, high));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMap) {
    int fahrenheit_freeze = 32;
    int fahrenheit_boil = 212;
    int celsius_freeze = 0;
    int celsius_boil = 100;

    //celcius to fahrenheit
    ASSERT_EQ(32, ra::generics::Map<int>(0, celsius_freeze, celsius_boil, fahrenheit_freeze, fahrenheit_boil));
    ASSERT_EQ(212, ra::generics::Map<int>(100, celsius_freeze, celsius_boil, fahrenheit_freeze, fahrenheit_boil));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testNear) {
    //as integer
    ASSERT_FALSE(ra::generics::Near(5, 9, 1));    //epsilon too small
    ASSERT_FALSE(ra::generics::Near(9, 5, 1));    //epsilon too small
    ASSERT_TRUE(ra::generics::Near(5, 9, 4));    //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(9, 5, 4));    //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(5, 9, 400));  //epsilon too big
    ASSERT_TRUE(ra::generics::Near(9, 5, 400));  //epsilon too big

    //as float
    ASSERT_FALSE(ra::generics::Near(5.0f, 9.0f, 1.0f));   //epsilon too small
    ASSERT_FALSE(ra::generics::Near(9.0f, 5.0f, 1.0f));   //epsilon too small
    ASSERT_TRUE(ra::generics::Near(5.0f, 9.0f, 4.0f));   //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(9.0f, 5.0f, 4.0f));   //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(5.0f, 9.0f, 400.0f)); //epsilon too big
    ASSERT_TRUE(ra::generics::Near(9.0f, 5.0f, 400.0f)); //epsilon too big

    //as double
    ASSERT_FALSE(ra::generics::Near(5.0, 9.0, 1.0));    //epsilon too small
    ASSERT_FALSE(ra::generics::Near(9.0, 5.0, 1.0));    //epsilon too small
    ASSERT_TRUE(ra::generics::Near(5.0, 9.0, 4.0));    //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(9.0, 5.0, 4.0));    //epsilon is exact
    ASSERT_TRUE(ra::generics::Near(5.0, 9.0, 400.0));  //epsilon too big
    ASSERT_TRUE(ra::generics::Near(9.0, 5.0, 400.0));  //epsilon too big
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testReadAs) {
    //float (32 bit) to integer (32 bit)
    {
      float f = 0.0f;
      int32_t i = 0;
      ASSERT_EQ(i, ra::generics::ReadAs<int32_t>(f));

      f = 0.1f;
      i = 1036831949;
      ASSERT_EQ(i, ra::generics::ReadAs<int32_t>(f));

      f = 100000.0f;
      i = 1203982336;
      ASSERT_EQ(i, ra::generics::ReadAs<int32_t>(f));

      f = 1.3958644e+010f;
      i = 1347420160;
      ASSERT_EQ(i, ra::generics::ReadAs<int32_t>(f));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testReverseOrder) {
    //test reverse even (6 elements)
    {
      ra::strings::StringVector names;
      names.push_back("Paul Atreides");
      names.push_back("Duke Leto Atreides");
      names.push_back("Lady Jessica");
      names.push_back("Stilgar");
      names.push_back("Baron Vladimir Harkonnen");
      names.push_back("Feyd Rautha");

      ra::strings::StringVector reverse_names;
      ra::generics::ReverseVector(names, reverse_names);

      ASSERT_EQ(std::string("Feyd Rautha"), reverse_names[0]);
      ASSERT_EQ(std::string("Baron Vladimir Harkonnen"), reverse_names[1]);
      ASSERT_EQ(std::string("Stilgar"), reverse_names[2]);
      ASSERT_EQ(std::string("Lady Jessica"), reverse_names[3]);
      ASSERT_EQ(std::string("Duke Leto Atreides"), reverse_names[4]);
      ASSERT_EQ(std::string("Paul Atreides"), reverse_names[5]);
    }

    //test reverse odd (5 elements)
    {
      ra::strings::StringVector names;
      names.push_back("Marty McFly");
      names.push_back("Doctor Emmett Brown");
      names.push_back("Biff Tannen");
      names.push_back("George McFly");
      names.push_back("Jennifer");

      ra::strings::StringVector reverse_names;
      ra::generics::ReverseVector(names, reverse_names);

      ASSERT_EQ(std::string("Jennifer"), reverse_names[0]);
      ASSERT_EQ(std::string("George McFly"), reverse_names[1]);
      ASSERT_EQ(std::string("Biff Tannen"), reverse_names[2]);
      ASSERT_EQ(std::string("Doctor Emmett Brown"), reverse_names[3]);
      ASSERT_EQ(std::string("Marty McFly"), reverse_names[4]);
    }

    //test empty list
    {
      ra::strings::StringVector names;
      ra::strings::StringVector reverse_names;
      ra::generics::ReverseVector(names, reverse_names);
    }

    //test single element list
    {
      ra::strings::StringVector names;
      names.push_back("HAL 9000");

      ra::strings::StringVector reverse_names;
      ra::generics::ReverseVector(names, reverse_names);

      ASSERT_EQ(std::string("HAL 9000"), reverse_names[0]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindMaxIndex) {
    //test default behavior
    {
      std::vector<int> values;
      values.push_back(1);
      values.push_back(22);
      values.push_back(42);
      values.push_back(-10);
      values.push_back(0);

      size_t index = ra::generics::FindMaxIndex(values);
      ASSERT_EQ(2, index);
    }

    //test empty list
    {
      std::vector<int> values;
      size_t index = ra::generics::FindMaxIndex(values);
      ASSERT_EQ(ra::generics::INVALID_INDEX, index);
    }

    //test single element list
    {
      std::vector<int> values;
      values.push_back(42);

      size_t index = ra::generics::FindMaxIndex(values);
      ASSERT_EQ(0, index);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindMinIndex) {
    //test default behavior
    {
      std::vector<int> values;
      values.push_back(1);
      values.push_back(22);
      values.push_back(42);
      values.push_back(-10);
      values.push_back(0);

      size_t index = ra::generics::FindMinIndex(values);
      ASSERT_EQ(3, index);
    }

    //test empty list
    {
      std::vector<int> values;
      size_t index = ra::generics::FindMinIndex(values);
      ASSERT_EQ(ra::generics::INVALID_INDEX, index);
    }

    //test single element list
    {
      std::vector<int> values;
      values.push_back(42);

      size_t index = ra::generics::FindMinIndex(values);
      ASSERT_EQ(0, index);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testCopyUnique) {
    //test default behavior
    {
      ra::strings::StringVector values;
      values.push_back("red");
      values.push_back("green");
      values.push_back("blue");
      values.push_back("white");
      values.push_back("white");
      values.push_back("red");
      values.push_back("green");
      values.push_back("white");
      values.push_back("yellow");
      values.push_back("blue");

      ra::strings::StringVector unique_values = ra::generics::CopyUnique(values);
      ASSERT_EQ(5, unique_values.size());

      //assert order is preserved
      ASSERT_EQ(std::string("red"), unique_values[0]);
      ASSERT_EQ(std::string("green"), unique_values[1]);
      ASSERT_EQ(std::string("blue"), unique_values[2]);
      ASSERT_EQ(std::string("white"), unique_values[3]);
      ASSERT_EQ(std::string("yellow"), unique_values[4]);
    }

    //test empty list
    {
      ra::strings::StringVector values;
      ra::strings::StringVector unique_values = ra::generics::CopyUnique(values);
      ASSERT_EQ(0, unique_values.size());
    }

    //test single element list
    {
      ra::strings::StringVector values;
      values.push_back("pink");

      ra::strings::StringVector unique_values = ra::generics::CopyUnique(values);
      ASSERT_EQ(1, unique_values.size());

      //assert order is preserved
      ASSERT_EQ(std::string("pink"), unique_values[0]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testMakeUnique) {
    //test default behavior
    {
      ra::strings::StringVector values;
      values.push_back("Mercury");
      values.push_back("Venus");
      values.push_back("Earth");
      values.push_back("Mars");
      values.push_back("Mars");
      values.push_back("Mars");
      values.push_back("Venus");
      values.push_back("Jupiter");
      values.push_back("Saturn");
      values.push_back("Uranus");
      values.push_back("Earth");
      values.push_back("Neptune");
      values.push_back("Pluto");
      values.push_back("Neptune");
      values.push_back("Uranus");

      size_t num_duplicates = ra::generics::MakeUnique(values);
      ASSERT_EQ(9, values.size());
      ASSERT_EQ(6, num_duplicates);

      //assert order is preserved
      ASSERT_EQ(std::string("Mercury"), values[0]);
      ASSERT_EQ(std::string("Venus"), values[1]);
      ASSERT_EQ(std::string("Earth"), values[2]);
      ASSERT_EQ(std::string("Mars"), values[3]);
      ASSERT_EQ(std::string("Jupiter"), values[4]);
      ASSERT_EQ(std::string("Saturn"), values[5]);
      ASSERT_EQ(std::string("Uranus"), values[6]);
      ASSERT_EQ(std::string("Neptune"), values[7]);
      ASSERT_EQ(std::string("Pluto"), values[8]);
    }

    //test empty list
    {
      ra::strings::StringVector values;
      size_t num_duplicates = ra::generics::MakeUnique(values);
      ASSERT_EQ(0, values.size());
      ASSERT_EQ(0, num_duplicates);
    }

    //test single element list
    {
      ra::strings::StringVector values;
      values.push_back("Earth");

      size_t num_duplicates = ra::generics::MakeUnique(values);
      ASSERT_EQ(1, values.size());
      ASSERT_EQ(0, num_duplicates);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindMaxValue) {
    //test default behavior
    {
      std::vector<int> values;
      values.push_back(1);
      values.push_back(22);
      values.push_back(42);
      values.push_back(-10);
      values.push_back(0);

      const int * max_value = ra::generics::FindMaxValue(values);
      ASSERT_TRUE(max_value != NULL);
      ASSERT_EQ(42, (*max_value));
    }

    //test empty list
    {
      std::vector<int> values;
      const int * max_value = ra::generics::FindMaxValue(values);
      ASSERT_TRUE(max_value == NULL);
    }

    //test single element list
    {
      std::vector<int> values;
      values.push_back(42);

      const int * max_value = ra::generics::FindMaxValue(values);
      ASSERT_TRUE(max_value != NULL);
      ASSERT_EQ(42, (*max_value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindMinValue) {
    //test default behavior
    {
      std::vector<int> values;
      values.push_back(1);
      values.push_back(22);
      values.push_back(42);
      values.push_back(-10);
      values.push_back(0);

      const int * min_value = ra::generics::FindMinValue(values);
      ASSERT_TRUE(min_value != NULL);
      ASSERT_EQ(-10, (*min_value));
    }

    //test empty list
    {
      std::vector<int> values;
      const int * min_value = ra::generics::FindMinValue(values);
      ASSERT_TRUE(min_value == NULL);
    }

    //test single element list
    {
      std::vector<int> values;
      values.push_back(42);

      const int * min_value = ra::generics::FindMinValue(values);
      ASSERT_TRUE(min_value != NULL);
      ASSERT_EQ(42, (*min_value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindIndexOf) {
    //test default behavior
    {
      static const std::string gods[] = {
        "Ymir",
        "Odin",
        "Frigg",
        "Thor",
        "Balder",
        "Tyr",
        "Bragi",
        "Loki",
        "Hel",
        "Heimdall",
        "Freyr",
        "Freya",
      };
      static const size_t num_gods = sizeof(gods) / sizeof(gods[0]);

      std::vector<size_t> locations = ra::generics::FindIndexOf<std::string>("Thor", gods, num_gods);
      ASSERT_EQ(1, locations.size());
      ASSERT_EQ(3, locations[0]);
    }

    //test no results
    {
      static const std::string gods[] = {
        "Ymir",
        "Odin",
        "Frigg",
        "Thor",
        "Balder",
        "Tyr",
        "Bragi",
        "Loki",
        "Hel",
        "Heimdall",
        "Freyr",
        "Freya",
      };
      static const size_t num_gods = sizeof(gods) / sizeof(gods[0]);

      std::vector<size_t> locations = ra::generics::FindIndexOf<std::string>("The strongest god", gods, num_gods);
      ASSERT_EQ(0, locations.size());
    }

    //test empty list
    {
      static const std::string gods[] = { "temp" };
      std::vector<size_t> locations = ra::generics::FindIndexOf<std::string>("Thor", gods, 0);
      ASSERT_EQ(0, locations.size());
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestGenerics, testFindFirst) {
    static const std::string gods[] = {
      "Anubis",
      "Horus",
      "Osiris",
      "Ra",
      "Set",
    };
    static const size_t num_gods = sizeof(gods) / sizeof(gods[0]);

    //test default behavior
    {
      size_t index = ra::generics::FindFirst<std::string>("Osiris", gods, num_gods);
      ASSERT_EQ(2, index);
    }

    //test not found
    {
      size_t index = ra::generics::FindFirst<std::string>("Bast", gods, num_gods);
      ASSERT_EQ(ra::generics::INVALID_INDEX, index);
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace generics
} //namespace ra
