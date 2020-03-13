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

#include "TestDemo.h"
#include "rapidassist/testing.h"
#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"

namespace ra { namespace test {
  void TestDemo::SetUp() {
    ASSERT_TRUE(ra::testing::CreateFile("demo1.tmp"));
    ASSERT_TRUE(ra::testing::CreateFile("demo2.tmp"));
  }

  void TestDemo::TearDown() {
    //cleanup
    ra::filesystem::DeleteFile("demo1.tmp");
    ra::filesystem::DeleteFile("demo2.tmp");
  }

  std::string generateTestFile() {
    //for testing purpose only
    return std::string("demo1.tmp");
  }
  std::string getExpectedTestFilePath() {
    //for testing purpose only
    return std::string("demo2.tmp");
  }

  TEST_F(TestDemo, testCodeSample) {
    //create a dummy file based on current gtest name
    static const int FILE_SIZE = 1000; //bytes
    const std::string path = ra::testing::GetTestQualifiedName() + ".tmp"; //returns "TestDemo.testCodeSample.tmp"
    ASSERT_TRUE(ra::testing::CreateFile(path.c_str(), FILE_SIZE));

    //test that a generated file is equals to the expected file
    std::string generatedFile = generateTestFile();
    std::string expectedFile = getExpectedTestFilePath();
    ASSERT_TRUE(ra::testing::IsFileEquals(expectedFile.c_str(), generatedFile.c_str()));

    //split a string into multiple parts
    ra::strings::StringVector words = ra::strings::Split("The quick brown fox jumps over the lazy dog", " ");
    size_t numWords = words.size(); //returns 9

    //converting numeric values to string
    std::string IntMaxStr = ra::strings::ToString(UINT64_MAX); //returns "18446744073709551615"

    //execute search and replace in strings
    std::string whoiam = "My name is Antoine and I am a superhero.";
    int numReplaced = ra::strings::Replace(whoiam, "hero", "vilan"); //returns 1

    //cleanup
    ra::filesystem::DeleteFile(path.c_str());
  }

} //namespace test
} //namespace ra
