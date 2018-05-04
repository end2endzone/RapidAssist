# RapidAssist #
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/rapidassist.svg)](https://github.com/end2endzone/rapidassist/releases)

RapidAssist is a lite cross-platform library that assist you with the most c++ repetitive task.



## Status ##

Build:

| Service | Build | Tests |
|----|-------|-------|
| AppVeyor | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/RapidAssist/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/rapidassist) | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/rapidassist/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master/tests) |
| Travis CI | [![Build Status](https://img.shields.io/travis/end2endzone/RapidAssist/master.svg?logo=travis&style=flat)](https://travis-ci.org/end2endzone/RapidAssist) |  |

Statistics:

| AppVeyor | Travic CI |
|----------|-----------|
| [![Statistics](https://buildstats.info/appveyor/chart/end2endzone/rapidassist)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master) | [![Statistics](https://buildstats.info/travisci/chart/end2endzone/RapidAssist)](https://travis-ci.org/end2endzone/RapidAssist) |




# Usage #

The following instructions show how to use the library.



## Source code example ##
The following section shows an example of using RapidAssist.

Assume a developer needs to test the following library function:
```cpp
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
```




# Build #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platform #

RapidAssist has been tested with the following platform:

*   Linux x86/x64
*   Windows x86/x64




# Versioning #

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/RapidAssist/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/RapidAssist/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
