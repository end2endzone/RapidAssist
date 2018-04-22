#include "TestEnvironment.h"
#include "environment.h"

namespace ra { namespace environment { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetEnvironmentVariable)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable(NULL);
      ASSERT_EQ(EXPECTED, actual);
    }

    //test empty string
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("");
      ASSERT_EQ(EXPECTED, actual);
    }

    //test not found
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("FOO_BAR_BIG_BANG");
      ASSERT_EQ(EXPECTED, actual);
    }

#ifdef WIN32
    //test TEMP
    {
      std::string actual = environment::getEnvironmentVariable("TEMP");
      ASSERT_NE("", actual);
    }
#elif UNIX
    //test SHELL
    {
      std::string actual = environment::getEnvironmentVariable("SHELL");
      ASSERT_NE("", actual);
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
