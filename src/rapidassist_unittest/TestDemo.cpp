#include "TestDemo.h"
#include "gtesthelper.h"
#include "strings.h"

using namespace ra::strings;

namespace ra { namespace test {

  void TestDemo::SetUp()
  {
  }

  void TestDemo::TearDown()
  {
  }

  std::string generateTestFile()
  {
    //for testing purpose only
    return std::string(__FILE__);
  }
  std::string getExpectedTestFilePath()
  {
    //for testing purpose only
    return std::string(__FILE__);
  }

  TEST_F(TestDemo, testCodeSample)
  {
    gTestHelper & hlp = gTestHelper::getInstance();
  
    //create a dummy file
    static const int FILE_SIZE = 1000; //bytes
    const std::string path = hlp.getTestQualifiedName() + ".tmp"; //returns "TestDemo.testCodeSample.tmp"
    ASSERT_TRUE( hlp.createFile(path.c_str(), FILE_SIZE) );
  
    //test that a generated file is equals to the expected file
    std::string generatedFile = generateTestFile();
    std::string expectedFile = getExpectedTestFilePath();
    ASSERT_TRUE ( hlp.isFileEquals( expectedFile.c_str(), generatedFile.c_str()) );

    //split a string into multiple parts
    StringVector words = splitString("The quick brown fox jumps over the lazy dog", " ");
    size_t numWords = words.size();

    //converting numeric values to string
    std::string IntMaxStr = ra::strings::toString(UINT64_MAX); //returns "18446744073709551615"

    //search and replace in strings
    std::string whoiam = "My name is Antoine and I am a superhero";
    int numReplaced = strReplace(whoiam, "hero", "vilan");
  }

} //namespace test
} //namespace ra
