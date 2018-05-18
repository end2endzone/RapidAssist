#include "TestGTestHelp.h"
#include "rapidassist/gtesthelp.h"

namespace ra { namespace test {

  void TestGTestHelp::SetUp()
  {
  }

  void TestGTestHelp::TearDown()
  {
  }

  TEST_F(TestGTestHelp, testFile1NotFound)
  {
    std::string msg;
    const char * file1 = "C:\\TEMP\\notfound.tmp";
    const char * file2 = __FILE__;
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is not found."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testFile2NotFound)
  {
    std::string msg;
    const char * file1 = __FILE__;
    const char * file2 = "C:\\TEMP\\notfound.tmp";
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("Second file is not found."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testFile1Smaller)
  {
    std::string msg;
    const std::string file1 = ra::gtesthelp::getTestQualifiedName() + ".1.tmp";
    const std::string file2 = ra::gtesthelp::getTestQualifiedName() + ".2.tmp";

    ASSERT_TRUE( ra::gtesthelp::createFile(file1.c_str(), 1000) );
    ASSERT_TRUE( ra::gtesthelp::createFile(file2.c_str(), 1200) );

    bool equals = ra::gtesthelp::isFileEquals(file1.c_str(), file2.c_str(), msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is smaller than Second file"), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testFile1Bigger)
  {
    std::string msg;
    const std::string file1 = ra::gtesthelp::getTestQualifiedName() + ".1.tmp";
    const std::string file2 = ra::gtesthelp::getTestQualifiedName() + ".2.tmp";

    ASSERT_TRUE( ra::gtesthelp::createFile(file1.c_str(), 1200) );
    ASSERT_TRUE( ra::gtesthelp::createFile(file2.c_str(), 1000) );

    bool equals = ra::gtesthelp::isFileEquals(file1.c_str(), file2.c_str(), msg);
    ASSERT_FALSE(equals) << msg.c_str();
    ASSERT_NE(msg.find("First file is bigger than Second file"), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testBigFileEquals)
  {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::gtesthelp::createFile(file1, 103000);
    ra::gtesthelp::createFile(file2, 103000);
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg);
    ASSERT_TRUE(equals) << msg.c_str();
    ASSERT_EQ(msg, "") << msg.c_str();
  }

  TEST_F(TestGTestHelp, testSmallFileEquals)
  {
    std::string msg;
    const char * file1 = __FILE__;
    const char * file2 = __FILE__;
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg);
    ASSERT_TRUE(equals) << msg.c_str();
    ASSERT_EQ(msg, "") << msg.c_str();
  }

  TEST_F(TestGTestHelp, testFileDiff3)
  {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::gtesthelp::createFile(file1, 10300);
    ra::gtesthelp::createFile(file2, 10300);
    ra::gtesthelp::changeFileContent(file1, 10, 0x01);
    ra::gtesthelp::changeFileContent(file1, 100, 0x10);
    ra::gtesthelp::changeFileContent(file1, 1027, 0xaa);
    ra::gtesthelp::changeFileContent(file1, 10270, 0xaa);
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg, 3);
    ASSERT_FALSE(equals) << msg.c_str();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d(0x%X)", 10, 10);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 100, 100);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 1027, 1027);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert 4th error not specified
    sprintf(buffer, "%d(0x%X)", 10270, 10270);
    ASSERT_EQ(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert more than 3 errors found
    ASSERT_NE(msg.find("..."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testFileDiffAll)
  {
    std::string msg;
    const char * file1 = "test1.bin";
    const char * file2 = "test2.bin";
    ra::gtesthelp::createFile(file1, 10300);
    ra::gtesthelp::createFile(file2, 10300);
    ra::gtesthelp::changeFileContent(file1, 10, 0x01);
    ra::gtesthelp::changeFileContent(file1, 100, 0x10);
    ra::gtesthelp::changeFileContent(file1, 1027, 0xaa);
    ra::gtesthelp::changeFileContent(file1, 10270, 0xaa);
    bool equals = ra::gtesthelp::isFileEquals(file1, file2, msg, 999);
    ASSERT_FALSE(equals) << msg.c_str();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d(0x%X)", 10, 10);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 100, 100);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 1027, 1027);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();
    sprintf(buffer, "%d(0x%X)", 10270, 10270);
    ASSERT_NE(msg.find(buffer), std::string::npos) << msg.c_str();

    //assert not more than 4 errors found
    ASSERT_EQ(msg.find("..."), std::string::npos) << msg.c_str();
  }

  TEST_F(TestGTestHelp, testGetTextFileContent)
  {
    ra::strings::StringVector lines;
    bool success = ra::gtesthelp::getTextFileContent( __FILE__, lines );
    ASSERT_TRUE(success);

    //assert a randomString found at line lineNumber
    static const char * randomString = "saldjsaldkj35knlkjxzlfnvskdnckzjshkaefhdkazjncslkirf"; int actualLineNumber = __LINE__; /* from 1 to n */
    int foundLineNumber = -1;
    for(size_t i=0; i<lines.size(); i++)
    {
      const std::string & line = lines[i];
      bool found = (line.find(randomString) != std::string::npos);
      if (found)
        foundLineNumber = (int)i + 1; //for 1 to n instead of 0 to n-1
    }
    ASSERT_EQ(foundLineNumber, actualLineNumber);
  }

} //namespace test
} //namespace ra
