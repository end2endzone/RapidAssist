
![RapidAssist logo](docs/RapidAssist-splashscreen.jpg?raw=true)


# RapidAssist #
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/rapidassist.svg)](https://github.com/end2endzone/rapidassist/releases)

RapidAssist is a lite cross-platform library that assist you with the most c++ repetitive task.



## Status ##

Build:

| Service/Platform    | Build                                                                                                                                                                                           | Tests                                                                                                                                                                                                                                                          |
|---------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| AppVeyor            | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/RapidAssist/master.svg?logo=AppVeyor&logoColor=white)](https://ci.appveyor.com/project/end2endzone/rapidassist)                 | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/rapidassist/master.svg?logo=AppVeyor&logoColor=white)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master/tests)                                                         |
| Travis CI           | [![Build Status](https://img.shields.io/travis/end2endzone/RapidAssist/master.svg?logo=Travis-CI&style=flat&logoColor=white)](https://travis-ci.org/end2endzone/RapidAssist)                    |                                                                                                                                                                                                                                                                |
| Windows Server 2019 | [![Build on Windows](https://github.com/end2endzone/RapidAssist/actions/workflows/build_windows.yml/badge.svg)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_windows.yml) | [![Tests on Windows](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/RapidAssist.master.Windows.json)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_windows.yml) |
| Ubuntu 20.04        | [![Build on Linux](https://github.com/end2endzone/RapidAssist/actions/workflows/build_linux.yml/badge.svg)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_linux.yml)       | [![Tests on Linux](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/RapidAssist.master.Linux.json)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_linux.yml)       |
| macOS 10.15         | [![Build on macOS](https://github.com/end2endzone/RapidAssist/actions/workflows/build_macos.yml/badge.svg)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_macos.yml)       | [![Tests on macOS](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/RapidAssist.master.macOS.json)](https://github.com/end2endzone/RapidAssist/actions/workflows/build_macos.yml)       |

Statistics:

| AppVeyor                                                                                                                                               | Travic CI                                                                                                                      | GitHub                                                                                                                             |
|--------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------|
| [![Statistics](https://buildstats.info/appveyor/chart/end2endzone/rapidassist)](https://ci.appveyor.com/project/end2endzone/rapidassist/branch/master) | [![Statistics](https://buildstats.info/travisci/chart/end2endzone/RapidAssist)](https://travis-ci.org/end2endzone/RapidAssist) | [![Statistics](https://buildstats.info/github/chart/end2endzone/rapidassist)](https://github.com/end2endzone/RapidAssist/actions/) |




# Philosophy #
Because of the cross-platform nature of the library, the code can get bulky pretty quick (or even messy in some case) because each different operating system has its own quirks and challenges.

The intention of RapidAssist is to be used as a library companion by providing a quick access to the most c++ repetitive tasks. The library aims to be _lite_, _easy to use_ and _intuitive_.

To get the code easy to understand, all functions are grouped by category and each category is defined by a namespace which helps increase cohesion between the code of each platform. The namespaces also help reduce coupling between categories.

The categories are `cli` (command line interface), `code`, `console`, `environment`, `filesystem`, `generics`, `testing`, `logging`, `process`, `random`, `strings`, `timing` and `unicode`.

The library does not intent to replace [Boost](https://www.boost.org/) or other full-featured cross-platform libraries.




# Usage #

The following instructions show how to use the library.



## Using RapidAssist in a CMake project ##

Using the library in other projects is relatively easy as the library supports the [find_package()](https://cmake.org/cmake/help/v3.4/command/find_package.html) command.
The following section explains how a client executable shall 'find' the RapidAssist library using `find_package()` command. For the example below, assume one wants to compile the `fooexe` executable target which have a dependency to the RapidAssist library.

To create a project that have a dependency on RapidAssist, the `CMakeLists.txt` configuration file should look like this:

```cmake
find_package(rapidassist 0.4.0 REQUIRED)
add_executable(fooexe <source_files> )
target_link_libraries(fooexe rapidassist)
```

Note that the `target_include_directories()` command is not required. The `fooexe` target will automatically have the `rapidassist` include directories assigned to the project.

In order for the `find_package()` command to automatically find the library, both RapidAssist and your project must be installed to the same installation directory.
However, on Windows, the CMake default installation directory is `C:\Program Files (x86)\${PROJECT_NAME}`. This makes the installation directory different for each projects. One must specify the RapidAssist's installation directory manually.
The installation path of RapidAssist can be specified manually by defining the `rapidassist_DIR` environment variable to RapidAssist installation directory. The `rapidassist_DIR` environment variable instruct CMake to use this directory while searching. By manually specifying the RapidAssist's install directory, the `find_package()` command will be able to resolve the path of RapidAssist and locate the include directory and library files.



## Source code example ##
The following section shows multiple examples of using RapidAssist.

```cpp
TEST_F(TestDemo, testCodeSample)
{
  //create a dummy file based on current gtest name
  static const int FILE_SIZE = 1000; //bytes
  const std::string path = ra::testing::getTestQualifiedName() + ".tmp"; //returns "TestDemo.testCodeSample.tmp"
  ASSERT_TRUE( ra::testing::createFile(path.c_str(), FILE_SIZE) );
  
  //test that a generated file is equals to the expected file
  std::string generatedFile = generateTestFile();
  std::string expectedFile = getExpectedTestFilePath();
  ASSERT_TRUE ( ra::testing::isFileEquals( expectedFile.c_str(), generatedFile.c_str()) );

  //split a string into multiple parts
  StringVector words = ra::strings::split("The quick brown fox jumps over the lazy dog", " ");
  size_t numWords = words.size(); //returns 9

  //converting numeric values to string
  std::string IntMaxStr = ra::strings::toString(UINT64_MAX); //returns "18446744073709551615"

  //execute search and replace in strings
  std::string whoiam = "My name is Antoine and I am a superhero.";
  int numReplaced = ra::strings::replace(whoiam, "hero", "vilan"); //returns 1
}
```




# Build #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platform #

RapidAssist has been tested with the following platform:

* Linux, 32 and 64 bit
* Windows, 32 and 64 bit
* macOS, 32 and 64 bit




# Versioning #

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/RapidAssist/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/RapidAssist/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
