#include "TestFilesystem.h"
#include "filesystem.h"
#include "time_.h"
#include "gtesthelper.h"

namespace ra { namespace filesystem { namespace test
{
  gTestHelper & helper = gTestHelper::getInstance();

  int countValues(const std::vector<std::string> & iList, const std::string & iValue)
  {
    int count = 0;
    for(size_t i=0; i<iList.size(); i++)
    {
      const std::string & value = iList[i];
      if (value == iValue)
        count++;
    }
    return count;
  }

  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestFilesystem::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileSize)
  {
    //test NULL
    {
      const char * path = NULL;
      uint32_t size = filesystem::getFileSize(path);
      ASSERT_EQ(0, size);
    }

    //test actual value
    {
      //create dummy file
      std::string filename = helper.getTestQualifiedName();
      ASSERT_TRUE( helper.createFile(filename.c_str()) );

#ifdef WIN32
      static const uint32_t EXPECTED = 14;
#elif __linux__
      static const uint32_t EXPECTED = 11;
#endif

      //test `const char *` api
      uint32_t size = filesystem::getFileSize(filename.c_str());
      ASSERT_EQ(EXPECTED, size);
      size = 0;

      //test `FILE*` api
      FILE * ptr = fopen(filename.c_str(), "r");
      ASSERT_TRUE(ptr != NULL);
      size = filesystem::getFileSize(ptr);
      fclose(ptr);
      ASSERT_EQ(EXPECTED, size);
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFilename)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string filename = filesystem::getFilename(NULL);
      ASSERT_EQ(EXPECTED, filename);
    }

    //test filename only
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (unix style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("/home/myFolder/foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }

    //test full path (windows style)
    {
      static const std::string EXPECTED = "foo.bar";
      std::string filename = filesystem::getFilename("C:\\Users\\Antoine\\Desktop\\myFolder\\foo.bar");
      ASSERT_EQ(EXPECTED, filename);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFileExists)
  {
    //test NULL
    {
      bool exists = filesystem::fileExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::fileExists("foo.bar.notfound.bang");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //create dummy file
      std::string filename = helper.getTestQualifiedName();
      ASSERT_TRUE( helper.createFile(filename.c_str()) );

      bool exists = filesystem::fileExists(filename.c_str());
      ASSERT_TRUE(exists);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testFolderExists)
  {
    //test NULL
    {
      bool exists = filesystem::folderExists(NULL);
      ASSERT_FALSE(exists);
    }

    //test not found
    {
      bool exists = filesystem::folderExists("/home/fooBAR");
      ASSERT_FALSE(exists);
    }

    //test found
    {
      //get dummy folder
      std::string currentFolder = filesystem::getCurrentFolder();
      ASSERT_TRUE( !currentFolder.empty() );

      bool exists = filesystem::folderExists(currentFolder.c_str());
      ASSERT_TRUE(exists);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFileName)
  {
    //test not empty
    {
      std::string filename = filesystem::getTemporaryFileName();
      ASSERT_TRUE( !filename.empty() );
    }

    //test repetitive
    {
      std::vector<std::string> filenames;
      static const size_t numTest = 20;
      for(size_t i=0; i<numTest; i++)
      {
        std::string filename = filesystem::getTemporaryFileName();
        filenames.push_back(filename);
      }

      //assert that all values are unique
      for(size_t i=0; i<numTest; i++)
      {
        const std::string & filename = filenames[i];
        int count = countValues(filenames, filename);
        ASSERT_EQ(1, count) << "Found value '" << filename << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetTemporaryFilePath)
  {
    //test not empty
    {
      std::string path = filesystem::getTemporaryFilePath();
      ASSERT_TRUE( !path.empty() );
    }

    //test repetitive
    {
      std::vector<std::string> paths;
      static const size_t numTest = 20;
      for(size_t i=0; i<numTest; i++)
      {
        std::string path = filesystem::getTemporaryFilePath();
        paths.push_back(path);
      }

      //assert that all values are unique
      for(size_t i=0; i<numTest; i++)
      {
        const std::string & path = paths[i];
        int count = countValues(paths, path);
        ASSERT_EQ(1, count) << "Found value '" << path << "' " << count << " times in the list.";
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetParentPath)
  {
    //test no folder
    {
      static const std::string EXPECTED = "";
      std::string parent = filesystem::getParentPath("filename.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test unix style
    {
      static const std::string EXPECTED = "/home/myFolder";
      std::string parent = filesystem::getParentPath("/home/myFolder/foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }

    //test windows style
    {
      static const std::string EXPECTED = "C:\\Users\\Antoine\\Desktop\\myFolder";
      std::string parent = filesystem::getParentPath("C:\\Users\\Antoine\\Desktop\\myFolder\\foo.bar");
      ASSERT_EQ(EXPECTED, parent);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetShortPathForm)
  {
    //test spaces in filename
    {
      static const std::string EXPECTED = "ABC~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("a b c.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long file extension
    {
      static const std::string EXPECTED = "ABCDEF~1.TEX";
      std::string shortPath = filesystem::getShortPathForm("abcdefgh.text");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test too long filename
    {
      static const std::string EXPECTED = "ABCDEF~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("abcdefghijklmnopqrstuvwxyz.txt");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test spaces in file extension
    {
      static const std::string EXPECTED = "ABCDE~1.TXT";
      std::string shortPath = filesystem::getShortPathForm("abcde.t x t");
      ASSERT_EQ(EXPECTED, shortPath);
    }

    //test program files (windows style)
    {
      static const std::string EXPECTED = "PROGRA~1";
      std::string shortPath = filesystem::getShortPathForm("Program Files (x86)");
      ASSERT_EQ(EXPECTED, shortPath);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPath)
  {
    //test baseline
    {
      static const std::string EXPECTED_PARENT = "/home/myFolder";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::splitPath("/home/myFolder/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test empty
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "";
      std::string parent, filename;
      filesystem::splitPath("", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test single filename
    {
      static const std::string EXPECTED_PARENT = "";
      static const std::string EXPECTED_FILENAME = "myfile.txt";
      std::string parent, filename;
      filesystem::splitPath("myfile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test spaces in folder
    {
      static const std::string EXPECTED_PARENT = "/home/my Folder";
      static const std::string EXPECTED_FILENAME = "myFile.txt";
      std::string parent, filename;
      filesystem::splitPath("/home/my Folder/myFile.txt", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    //test filename without extension / folder name
    {
      static const std::string EXPECTED_PARENT = "/home/myFolder";
      static const std::string EXPECTED_FILENAME = "myFile";
      std::string parent, filename;
      filesystem::splitPath("/home/myFolder/myFile", parent, filename);
      ASSERT_EQ(EXPECTED_PARENT, parent);
      ASSERT_EQ(EXPECTED_FILENAME, filename);
    }

    std::string folder;
    std::string filename;
  
    //from full path
    {
      splitPath("C:\\foo\\bar\\file.txt", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      splitPath("C:\\foo\\bar\\file", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file");
    }
    {
      splitPath("C:\\foo\\bar\\file.", folder, filename);
      ASSERT_EQ(folder, "C:\\foo\\bar");
      ASSERT_EQ(filename, "file.");
    }

    //from filename only
    {
      splitPath("file.txt", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file.txt");
    }
    {
      splitPath("file.", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file.");
    }
    {
      splitPath("file", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "file");
    }

    //empty strings
    {
      splitPath("", folder, filename);
      ASSERT_EQ(folder, "");
      ASSERT_EQ(filename, "");
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testSplitPathArray)
  {
    //test baseline
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/myFolder/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myFolder", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test empty
    {
      std::vector<std::string> elements;
      filesystem::splitPath("", elements);
      ASSERT_EQ(0, elements.size());
    }

    //test single filename
    {
      std::vector<std::string> elements;
      filesystem::splitPath("myfile.txt", elements);
      ASSERT_EQ(1, elements.size());
      ASSERT_EQ("myfile.txt", elements[0]);
    }

    //test spaces in folder
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/my Folder/myFile.txt", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("my Folder", elements[1]);
      ASSERT_EQ("myFile.txt", elements[2]);
    }

    //test filename without extension / folder name
    {
      std::vector<std::string> elements;
      filesystem::splitPath("/home/myFolder/myFile", elements);
      ASSERT_EQ(3, elements.size());
      ASSERT_EQ("home", elements[0]);
      ASSERT_EQ("myFolder", elements[1]);
      ASSERT_EQ("myFile", elements[2]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetPathSeparator)
  {
#ifdef WIN32
      ASSERT_EQ('\\', filesystem::getPathSeparator());
#elif __linux__
      ASSERT_EQ('/', filesystem::getPathSeparator());
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetCurrentFolder)
  {
    std::string curdir = getCurrentFolder();
    ASSERT_NE("", curdir);

    ASSERT_TRUE(filesystem::folderExists(curdir.c_str()));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileExtention)
  {
    //test baseline
    {
      static const std::string EXPECTED = "txt";
      std::string ext = filesystem::getFileExtention("myFile.txt");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test empty
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test spaces in extension
    {
      static const std::string EXPECTED = "foo bar";
      std::string ext = filesystem::getFileExtention("/home/my Folder/myFile.foo bar");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test filename without extension / folder name
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("/home/myFolder/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //test folder with an extension and file without extension
    {
      static const std::string EXPECTED = "";
      std::string ext = filesystem::getFileExtention("/home/my.Folder/myFile");
      ASSERT_EQ(EXPECTED, ext);
    }

    //from valid filename
    {
      std::string ext = getFileExtention("file.txt");
      ASSERT_EQ(ext, "txt");
    }
    {
      std::string ext = getFileExtention("file.");
      ASSERT_EQ(ext, "");
    }
    {
      std::string ext = getFileExtention("file");
      ASSERT_EQ(ext, "");
    }

    //from full path
    {
      std::string ext = getFileExtention("C:\\foo\\bar\\file.txt");
      ASSERT_EQ(ext, "txt");
    }

    //from filename with multiple dots
    {
      std::string ext = getFileExtention("file.subfile.txt");
      ASSERT_EQ(ext, "txt");
    }

    //empty strings
    {
      std::string ext = getFileExtention("");
      ASSERT_EQ(ext, "");
    }

    //from special case path
    {
      std::string ext = getFileExtention("C:\\foo.bar\\file");
      ASSERT_EQ(ext, "");
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetUserFriendlySize)
  {
    static const uint64_t MULTIPLICATOR_KB = 1024;
    static const uint64_t MULTIPLICATOR_MB = 1024*MULTIPLICATOR_KB;
    static const uint64_t MULTIPLICATOR_GB = 1024*MULTIPLICATOR_MB;
    static const uint64_t MULTIPLICATOR_TB = 1024*MULTIPLICATOR_GB;

    {
      static const std::string EXPECTED = "0 bytes";
      std::string size = filesystem::getUserFriendlySize(0ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1023 bytes";
      std::string size = filesystem::getUserFriendlySize(1023ull);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 KB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 MB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_KB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 MB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 GB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_MB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 GB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "0.97 TB";
      std::string size = filesystem::getUserFriendlySize(1000ull*MULTIPLICATOR_GB);
      ASSERT_EQ(EXPECTED, size);
    }
    {
      static const std::string EXPECTED = "1.00 TB";
      std::string size = filesystem::getUserFriendlySize(1ull*MULTIPLICATOR_TB);
      ASSERT_EQ(EXPECTED, size);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestFilesystem, testGetFileModifiedDate)
  {
    //assert that unit of return value is seconds
    {
      static const uint64_t EXPECTED = 3;
      const std::string filename1 = helper.getTestQualifiedName() + ".1.txt";
      const std::string filename2 = helper.getTestQualifiedName() + ".2.txt";
      ASSERT_TRUE( helper.createFile(filename1.c_str()) );
      ra::time::millisleep(1000*EXPECTED + 50); //at least 3 seconds between the files
      ASSERT_TRUE( helper.createFile(filename2.c_str()) );

      uint64_t time1 = filesystem::getFileModifiedDate(filename1);
      uint64_t time2 = filesystem::getFileModifiedDate(filename2);
      uint64_t diff = time2 - time1;
      ASSERT_GT(diff, EXPECTED-1); //allow 1 seconds of difference
      ASSERT_LT(diff, EXPECTED+1); //allow 1 seconds of difference
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace filesystem
} //namespace ra
