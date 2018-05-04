#include "TestCli.h"
#include "cli.h"
#include "strings.h"

namespace ra { namespace cli { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestCli::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestCli::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestCli, testParseArgumentString)
  {
    const char * arguments[] = {"myapp", "--arg1=value1", "--test2=value2", "--quick", "--number=12345", "--big=4123456789", "--foo=bar", "--last=final", ""};
    int argc = sizeof(arguments)/sizeof(arguments[0]) - 1; //the last element must be NULL and not part of the argc value.

    //must const cast because argv is supplied as a `char**` instead of `const char **`
    char ** argv = (char**)arguments;

    //test flag/switch
    {
      static const std::string name = "quick";
      std::string value;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "");
    }

    //test first
    {
      static const std::string name = "arg1";
      std::string value;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "value1");
    }

    //test last
    {
      static const std::string name = "last";
      std::string value;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "final");
    }

    //test middle
    {
      static const std::string name = "foo";
      std::string value;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, "bar");
    }

    //test int
    {
      static const std::string name = "number";
      int value = 0;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, 12345);
    }

    //test size_t
    {
      static const std::string name = "big";
      size_t value = 0;
      bool found = cli::parseArgument(name.c_str(), value, argc, argv);
      ASSERT_TRUE(found);
      ASSERT_EQ(value, 4123456789ul);
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace cli
} //namespace ra
