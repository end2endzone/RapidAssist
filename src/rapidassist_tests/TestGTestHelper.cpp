#include "TestGTestHelper.h"
#include "gtesthelper.h"

gTestHelper & hlp = gTestHelper::getInstance();

void TestGTestHelper::SetUp()
{
}

void TestGTestHelper::TearDown()
{
}

TEST_F(TestGTestHelper, testFile1NotFound)
{
  std::string msg;
  const char * file1 = "C:\\TEMP\\notfound.tmp";
  const char * file2 = __FILE__;
  bool equals = hlp.isFileEquals(file1, file2, msg);
  ASSERT_FALSE(equals) << msg.c_str();
  ASSERT_NE(msg.find("First file is not found."), std::string::npos) << msg.c_str();
}

TEST_F(TestGTestHelper, testFile2NotFound)
{
  std::string msg;
  const char * file1 = __FILE__;
  const char * file2 = "C:\\TEMP\\notfound.tmp";
  bool equals = hlp.isFileEquals(file1, file2, msg);
  ASSERT_FALSE(equals) << msg.c_str();
  ASSERT_NE(msg.find("Second file is not found."), std::string::npos) << msg.c_str();
}

TEST_F(TestGTestHelper, testFile1Smaller)
{
  std::string msg;
  const std::string file1 = hlp.getTestQualifiedName() + ".1.tmp";
  const std::string file2 = hlp.getTestQualifiedName() + ".2.tmp";

  ASSERT_TRUE( hlp.createFile(file1.c_str(), 1000) );
  ASSERT_TRUE( hlp.createFile(file2.c_str(), 1200) );

  bool equals = hlp.isFileEquals(file1.c_str(), file2.c_str(), msg);
  ASSERT_FALSE(equals) << msg.c_str();
  ASSERT_NE(msg.find("First file is smaller than Second file"), std::string::npos) << msg.c_str();
}

TEST_F(TestGTestHelper, testFile1Bigger)
{
  std::string msg;
  const std::string file1 = hlp.getTestQualifiedName() + ".1.tmp";
  const std::string file2 = hlp.getTestQualifiedName() + ".2.tmp";

  ASSERT_TRUE( hlp.createFile(file1.c_str(), 1200) );
  ASSERT_TRUE( hlp.createFile(file2.c_str(), 1000) );

  bool equals = hlp.isFileEquals(file1.c_str(), file2.c_str(), msg);
  ASSERT_FALSE(equals) << msg.c_str();
  ASSERT_NE(msg.find("First file is bigger than Second file"), std::string::npos) << msg.c_str();
}

TEST_F(TestGTestHelper, testBigFileEquals)
{
  std::string msg;
  const char * file1 = "test1.bin";
  const char * file2 = "test2.bin";
  hlp.createFile(file1, 103000);
  hlp.createFile(file2, 103000);
  bool equals = hlp.isFileEquals(file1, file2, msg);
  ASSERT_TRUE(equals) << msg.c_str();
  ASSERT_EQ(msg, "") << msg.c_str();
}

TEST_F(TestGTestHelper, testSmallFileEquals)
{
  std::string msg;
  const char * file1 = __FILE__;
  const char * file2 = __FILE__;
  bool equals = hlp.isFileEquals(file1, file2, msg);
  ASSERT_TRUE(equals) << msg.c_str();
  ASSERT_EQ(msg, "") << msg.c_str();
}

TEST_F(TestGTestHelper, testFileDiff3)
{
  std::string msg;
  const char * file1 = "test1.bin";
  const char * file2 = "test2.bin";
  hlp.createFile(file1, 10300);
  hlp.createFile(file2, 10300);
  hlp.changeFileContent(file1, 10, 0x01);
  hlp.changeFileContent(file1, 100, 0x10);
  hlp.changeFileContent(file1, 1027, 0xaa);
  hlp.changeFileContent(file1, 10270, 0xaa);
  bool equals = hlp.isFileEquals(file1, file2, msg, 3);
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

TEST_F(TestGTestHelper, testFileDiffAll)
{
  std::string msg;
  const char * file1 = "test1.bin";
  const char * file2 = "test2.bin";
  hlp.createFile(file1, 10300);
  hlp.createFile(file2, 10300);
  hlp.changeFileContent(file1, 10, 0x01);
  hlp.changeFileContent(file1, 100, 0x10);
  hlp.changeFileContent(file1, 1027, 0xaa);
  hlp.changeFileContent(file1, 10270, 0xaa);
  bool equals = hlp.isFileEquals(file1, file2, msg, 999);
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

TEST_F(TestGTestHelper, testGetTextFileContent)
{
  gTestHelper::StringVector lines;
  bool success = hlp.getTextFileContent( __FILE__, lines );
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

TEST_F(TestGTestHelper, testFileExists)
{
  ASSERT_TRUE ( hlp.fileExists( __FILE__ ) );
  ASSERT_FALSE( hlp.fileExists( __FILE__"not exist" ) );
}