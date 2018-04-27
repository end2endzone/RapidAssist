#include "gtesthelper.h"
#include <sstream> //for stringstream
#include <iostream> //for std::hex
#include <cstdio> //for remove()
#include <gtest/gtest.h>

#include "filesystem.h"
#include "string_.h"
#include "environment.h"

using namespace ra::filesystem;
using namespace ra::strings;

namespace ra
{

  //
  // Description:
  //  Wrapper class for FILE* instance.
  //  Which automatically closes the FILE* on wrapper's destruction
  // 
  class FileWrapper
  {
  public:
    FileWrapper(const char * iPath, const char * iMode)
    {
      mPointer = fopen(iPath, iMode);
    }

    ~FileWrapper()
    {
      close();
    }

    bool isEOF()
    {
      if (mPointer == NULL)
        return true;
      //http://www.cplusplus.com/reference/cstdio/feof/
      return (feof(mPointer) != 0);
    }

    void close()
    {
      if (mPointer)
      {
        fclose(mPointer);
        mPointer = NULL;
      }
    }

    //members
    FILE * mPointer;
  };

  std::string subString2(const std::string & iString, size_t iStart, size_t iCount)
  {
    std::string tmp = iString;
    tmp.substr(iStart, iCount);
    return tmp;
  }

  gTestHelper::gTestHelper()
  {
  }

  gTestHelper::~gTestHelper()
  {
  }

  gTestHelper & gTestHelper::getInstance()
  {
    static gTestHelper instance;
    return instance;
  }

  //------------------------
  // Methods
  //------------------------
  bool gTestHelper::findArgument(const char * iName, std::string & oValue, int argc, char **argv)
  {
    //Build search pattern
    std::string pattern = "--";
    pattern.append(iName);
    pattern.append("=");

    for(int i=0; i<argc; i++)
    {
      std::string arg = argv[i];
      size_t pos = arg.find(pattern);
      if (pos != std::string::npos)
      {
        //found. extracting value
        oValue = subString2(arg, pos+1, 99999);
        return true;
      }
    }

    return false;
  }

  std::string gTestHelper::mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter)
  {
    return mergeFilter(iPositiveFilter, iNegativeFilter, NULL);
  }

  std::string gTestHelper::mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, int argc, char **argv)
  {
    //find supplied --gtest_filter argument
    std::string gtest_filter;
    bool found = findArgument("gtest_filter", gtest_filter, argc, argv);
    if (found)
      return mergeFilter(iPositiveFilter, iNegativeFilter, gtest_filter.c_str());
    return mergeFilter(iPositiveFilter, iNegativeFilter, NULL);
  }

  std::string gTestHelper::mergeFilter(const std::string & iPositiveFilter, const std::string & iNegativeFilter, const char * iExistingFilter)
  {
    std::string filter;

    std::string positiveFilter = iPositiveFilter;
    std::string negativeFilter = iNegativeFilter;

    if (iExistingFilter)
    {
      std::string argPositiveFilter;
      std::string argNegativeFilter;
      gTestHelper::splitFilter(iExistingFilter, argPositiveFilter, argNegativeFilter);

      //append argument filters to positiveFilter and negativeFilter filters
      if (positiveFilter == "")
        positiveFilter = argPositiveFilter;
      else if (argPositiveFilter == "" || argPositiveFilter == "*")
      {
        //nothing to add since argPositiveFilter does not specify test cases
      }
      else
      {
        positiveFilter.append(":");
        positiveFilter.append(argPositiveFilter);
      }
      if (negativeFilter == "")
        negativeFilter = argNegativeFilter;
      else if (argNegativeFilter == "" || argNegativeFilter == "*")
      {
        //nothing to add since argNegativeFilter does not specify test cases
      }
      else 
      {
        negativeFilter.append(":");
        negativeFilter.append(argNegativeFilter);
      }
    }

    if (positiveFilter != "" || negativeFilter != "")
    {
      if (positiveFilter == "")
        filter.append("*");
      else
        filter.append(positiveFilter);
      if (negativeFilter != "")
      {
        filter.append("-");
        filter.append(negativeFilter);
      }
    }

    return filter;
  }

  void gTestHelper::splitFilter(const char * iFilter, std::string & oPositiveFilter, std::string & oNegativeFilter)
  {
    oPositiveFilter = "";
    oNegativeFilter = "";

    if (iFilter == NULL)
      return;

    std::string filterString = iFilter;
    if (filterString == "")
      return;

    gTestHelper::StringVector filters;
    splitString(filters, iFilter, "-");

    if (filters.size() > 2)
    {
      //something went wrong filter contains multiple '-' character
      //return the first 2 elements either way
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = filters[1].c_str();
    }
    else if (filters.size() == 2)
    {
      //positive and negative filter found
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = filters[1].c_str();
    }
    else if (filters.size() == 1)
    {
      //positive only filter found
      oPositiveFilter = filters[0].c_str();
      oNegativeFilter = "";
    }
    else
    {
      //something went wrong
    }
  }

  gTestHelper::StringVector gTestHelper::getTestList(const char * iTestCasePath)
  {
    //check that file exists
    if (!fileExists(iTestCasePath))
      return StringVector();

    static const std::string logFilename = "gTestHelper.tmp";

    std::string commandLine;
    commandLine.append("cmd /c \"");
    commandLine.append("\"");
    commandLine.append(iTestCasePath);
    commandLine.append("\"");
    commandLine.append(" --gtest_list_tests");
    commandLine.append(" 2>NUL");
    commandLine.append(" 1>");
    commandLine.append("\"");
    commandLine.append(logFilename);
    commandLine.append("\"");

    //exec
    int returnCode = system(commandLine.c_str());
    if (returnCode != 0)
      return StringVector();

    if (!fileExists(logFilename.c_str()))
      return StringVector();

    //load test case list from log filename
    StringVector testlist;
    static const std::string disabledTestCaseHeader = "  DISABLED_";
    static const std::string disabledTestSuiteHeader = "DISABLED_";
    std::string testSuiteName;
    std::string testCaseName;
    FILE * f = fopen(logFilename.c_str(), "r");
    if (!f)
      return StringVector();

    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    while( fgets(buffer, BUFFER_SIZE, f) )
    {
      std::string line = buffer;
      line.substr(0, line.size()-1); //remove CRLF at the end of the 
      if (subString2(line, 0, disabledTestCaseHeader.size()) == disabledTestCaseHeader)
      {
        //do nothing
      }
      else if (subString2(line, 0, 2) == "  ")
      {
        //test case
        std::string fullTestCaseName;
        fullTestCaseName.append(testSuiteName);
        fullTestCaseName.append(subString2(line, 2, 999));
        testlist.push_back(fullTestCaseName);
      }
      else
      {
        //test suite name
        testSuiteName = "";
        if (subString2(line, 0, disabledTestSuiteHeader.size()) == disabledTestSuiteHeader)
        {
          //disabled test suite
        }
        else
        {
          testSuiteName = line;
        }
      }
    }

    fclose(f);

    //delete log file
    int removeResult = remove(logFilename.c_str());

    //exec
    returnCode = system(commandLine.c_str());

    return testlist;
  }

  bool gTestHelper::isFileEquals(const char* iFile1, const char* iFile2)
  {
    std::string reason;
    return isFileEquals(iFile1, iFile2, reason, 1 /*return ASAP*/ );
  }

  bool gTestHelper::isFileEquals(const char* iFile1, const char* iFile2, std::string & oReason)
  {
    return isFileEquals(iFile1, iFile2, oReason, 1 /*return ASAP*/ );
  }

  bool gTestHelper::isFileEquals(const char* iFile1, const char* iFile2, std::string & oReason, size_t iMaxDifferences)
  {
    //Build basic message
    oReason = "";
    std::stringstream ss;
    ss << "Comparing first file \"" << iFile1 << "\" with second file \"" << iFile2 << "\". ";

    FileWrapper f1(iFile1, "rb");
    if (f1.mPointer == NULL)
    {
      ss << "First file is not found.";
      oReason = ss.str();
      return false;
    }
    FileWrapper f2(iFile2, "rb");
    if (f2.mPointer == NULL)
    {
      ss << "Second file is not found.";
      oReason = ss.str();
      return false;
    }

    //Compare by size
    long size1 = getFileSize(f1.mPointer);
    long size2 = getFileSize(f2.mPointer);
    if (size1 != size2)
    {
      if (size1 < size2)
        ss << "First file is smaller than Second file: " << size1 << " vs " << size2 << ".";
      else
        ss << "First file is bigger than Second file: " << size1 << " vs " << size2 << ".";
      oReason = ss.str();
      return false;
    }

    //Compare content
    f1.close();
    f2.close();
    std::vector<FILE_DIFF> differences;
    bool success = getFileDifferences(iFile1, iFile2, differences, iMaxDifferences+1); //search 1 more record to differentiate between exactly iMaxDifferences differences and more than iMaxDifferences differences
    if (!success)
    {
      ss << "Unable to determine if content is identical...";
      oReason = ss.str();
      return false;
    }

    if (differences.size() == 0)
    {
      //no diffences. Files are identicals
      return true;
    }

    //Build error message from differences
    ss << "Content is different: ";
    for(size_t i=0; i<differences.size() && i<iMaxDifferences; i++)
    {
      const FILE_DIFF & d = differences[i];
      if (i >= 1)
        ss << ", ";
      static const int BUFFER_SIZE = 1024;
      char buffer[BUFFER_SIZE];
#ifdef _WIN32
      sprintf(buffer, "{address %Iu(0x%IX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#else
      sprintf(buffer, "{address %zu(0x%zX) is 0x%02X instead of 0x%02X}", d.offset, d.offset, d.c1, d.c2);
#endif
      ss << buffer;
      //ss << "{at offset " << (d.offset) << "(0x" << std::hex << (int)d.offset << ") has 0x" << std::hex << (int)d.c1 << " vs 0x" << std::hex << (int)d.c2 << "}";
    }
    if (differences.size() > iMaxDifferences)
      ss << ", ...";
    oReason = ss.str();
    return false;
  }

  bool gTestHelper::getFileDifferences(const char* iFile1, const char* iFile2, std::vector<FILE_DIFF> & oDifferences, size_t iMaxDifferences)
  {
    FileWrapper f1(iFile1, "rb");
    if (f1.mPointer == NULL)
      return false;
    FileWrapper f2(iFile2, "rb");
    if (f2.mPointer == NULL)
      return false;

    //Check by size
    long size1 = getFileSize(f1.mPointer);
    long size2 = getFileSize(f2.mPointer);
    if (size1 != size2)
    {
      return false; //unsupported
    }

    //Compare content
    static const size_t BUFFER_SIZE = 1024;
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    size_t offsetRead = 0;

    //while there is data to read in files
    while( !f1.isEOF() && !f2.isEOF() )
    {
      size_t readSize1 = fread(buffer1, 1, BUFFER_SIZE, f1.mPointer);
      size_t readSize2 = fread(buffer2, 1, BUFFER_SIZE, f2.mPointer);
      if (readSize1 != readSize2)
      {
        //this should not happend since both files are identical in length.
        return false; //failed
      }
      bool contentEquals = memcmp(buffer1, buffer2, readSize1) == 0;
      if (!contentEquals)
      {
        //current buffers are different

        //Find differences and build file diff info.
        for(size_t i = 0; i < BUFFER_SIZE; i++)
        {
          unsigned char c1 = (unsigned char)buffer1[i];
          unsigned char c2 = (unsigned char)buffer2[i];
          if (c1 != c2)
          {
            FILE_DIFF d;
            d.offset = offsetRead+i;
            d.c1 = c1;
            d.c2 = c2;
            oDifferences.push_back(d);

            //check max differences found
            if (oDifferences.size() == iMaxDifferences)
              return true;
          }
        }
      }
      offsetRead += readSize1;
    }
    return true;
  }

  bool gTestHelper::findInFile(const char* iFilename, const char* iValue, int & oLine, int & oCharacter)
  {
    if (!fileExists(iFilename))
      return false;

    oLine = -1;
    oCharacter = -1;

    gTestHelper::StringVector lines;
    bool success = getTextFileContent( iFilename, lines );
    if (!success)
      return false;

    for(size_t i=0; i<lines.size(); i++)
    {
      const std::string & line = lines[i];
      size_t position = line.find(iValue, 0);
      if (position != std::string::npos)
      {
        oLine = (int)i;
        oCharacter = (int)position;
        return true;
      }
    }

    return false;
  }

  bool gTestHelper::getTextFileContent(const char* iFilename, gTestHelper::StringVector & oLines )
  {
    oLines.clear();

    static const int BUFFER_SIZE = 10240;
    char buffer[BUFFER_SIZE];

    FILE* f = fopen(iFilename, "r");
    if (f)
    {
      while( fgets(buffer, BUFFER_SIZE, f) != NULL )
      {
        //remove last CRLF at the end of the string
        removeEOL(buffer);

        std::string line = buffer;
        oLines.push_back(line);
      }
      fclose(f);
      return true;
    }
    return false;
  }

  bool gTestHelper::createFile(const char * iFilePath, size_t iSize)
  {
    FILE * f = fopen(iFilePath, "wb");
    if (!f)
      return false;
    for(size_t i=0; i<iSize; i++)
    {
      unsigned int value = (i%256);
      fwrite(&value, 1, 1, f);
    }
    fclose(f);
    return true;
  }

  bool gTestHelper::createFile(const char * iFilePath)
  {
    FILE * f = fopen(iFilePath, "w");
    if (f == NULL)
      return false;
    fputs("FOO!\n", f);
    fputs("&\n", f);
    fputs("BAR\n", f);
    fclose(f);

    return true;
  }

  void gTestHelper::changeFileContent(const char * iFilePath, size_t iOffset, unsigned char iValue)
  {
    //read
    FILE * f = fopen(iFilePath, "rb");
    if (!f)
      return;
    long size = getFileSize(f);
    unsigned char * buffer = new unsigned char[size];
    if (!buffer)
      return;
    size_t byteRead = fread(buffer, 1, size, f);
    fclose(f);

    //modify
    if (iOffset < (size_t)size)
      buffer[iOffset] = iValue;

    //save
    f = fopen(iFilePath, "wb");
    if (!f)
      return;
    size_t byteWrite = fwrite(buffer, 1, size, f);
    fclose(f);
  }

  bool gTestHelper::isProcessorX86()
  {
    return environment::isProcess32Bit();
  }

  bool gTestHelper::isProcessorX64()
  {
    return environment::isProcess64Bit();
  }

  bool gTestHelper::isDebugCode()
  {
    return environment::isConfigurationDebug();
  }

  bool gTestHelper::isReleaseCode()
  {
    return environment::isConfigurationRelease();
  }

  bool gTestHelper::isAppVeyor()
  {
    return !environment::getEnvironmentVariable("APPVEYOR").empty();
  }

  bool gTestHelper::isTravis()
  {
    return !environment::getEnvironmentVariable("TRAVIS").empty();
  }

  bool gTestHelper::isJenkins()
  {
    return !environment::getEnvironmentVariable("JENKINS_URL").empty();
  }

  std::string gTestHelper::getTestSuiteName()
  {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    return name;
  }

  std::string gTestHelper::getTestCaseName()
  {
    std::string name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    return name;
  }

  std::string gTestHelper::getTestQualifiedName()
  {
    const char * testSuiteName = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    const char * testCaseName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

    std::string name;
    name.append(testSuiteName);
    name.append(".");
    name.append(testCaseName);

    return name;
  }

} //namespace ra
