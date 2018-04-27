[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/rapidassist.svg)](https://github.com/end2endzone/rapidassist/releases)

| Service | Build | Tests |
|----|-------|-------|
| AppVeyor | [![Build status](https://ci.appveyor.com/api/projects/status/tmi3dnkuwpguqngv?svg=true)](https://ci.appveyor.com/project/end2endzone/rapidassist) | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/rapidassist/master.svg)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master/tests) |
| Travic CI | [![Build Status](https://img.shields.io/travis/end2endzone/RapidAssist/master.svg?logo=travis&style=flat)](https://travis-ci.org/end2endzone/RapidAssist) | |


AppVeyor build statistics:

[![Build statistics](https://buildstats.info/appveyor/chart/end2endzone/rapidassist)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master)


# RapidAssist

RapidAssist is a lite cross-platform library that assist you with the most c++ repretitive task

# Usage

The following instructions show how to use the library.

## Source code example
The following section shows an example of using RapidAssist.

Assume a developer needs to test the following library function:
```cpp
#include "gtesthelper.h"

TEST_F(TestGTestHelper, testFileExists)
{
  gTestHelper & hlp = gTestHelper::getInstance();
  
  //create a dummy file
  static const int FILE_SIZE = 1000; //bytes
  const std::string path = hlp.getTestQualifiedName() + ".tmp";
  ASSERT_TRUE( hlp.createFile(path.c_str(), FILE_SIZE) );
  
  //assert that a file exists
  ASSERT_TRUE ( hlp.fileExists( __FILE__ ) );
  ASSERT_FALSE( hlp.fileExists( __FILE__"not exist" ) );
}
```

# Build / Install

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.

# Testing
RapidAssist comes with unit tests which help maintaining the product stability and level of quality.

Test are build using the Google Test v1.6.0 framework. For more information on how googletest is working, see the [google test documentation primer](https://github.com/google/googletest/blob/release-1.8.0/googletest/docs/V1_6_Primer.md).  

Test are automatically build when building the solution. See [INSTALL.md](INSTALL.md) for details on how to build the software.

To run tests, open a file navigator and browse to the output folder (for example `c:\projects\rapidassist\build\bin\Release`) and run `rapidassist_tests.exe` executable.

Test results are saved in junit format in file `rapidassist_unittest.x86.debug.xml` or `rapidassist_unittest.x86.release.xml` depending on the selected configuration.

See also the latest test results at the beginning of the document.

# Compatible with

RapidAssist is available for multiple platform:

*   Linux
*   Windows
*   Mac OS (soon)

# Versioning

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/RapidAssist/tags).

# Authors

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/RapidAssist/blob/master/AUTHORS) who participated in this project.

# License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details