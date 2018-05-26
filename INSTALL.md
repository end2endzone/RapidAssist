# Install #

The library does not provide an installation package. It is deployed using a zip archive which only contains source code. Other applications/libraries must compile the source code locally to use the library functionalities.

The following steps show how other projects can find and use the library:

1) Download the source code from an existing [tags](http://github.com/end2endzone/RapidAssist/tags) and extract the content to a local directory (for example `c:\projects\RapidAssist` or `~/dev/RapidAssist`).

2) Define the environment variable `RAPIDASSIST_HOME` to the location where the source code was extracted.



# Build #

This section explains how to compile and build the software and how to get a development environment ready.



## Prerequisites ##

### Software Requirements ###
The following software must be installed on the system for compiling source code:

* [Google C++ Testing Framework v1.6.0](https://github.com/google/googletest/tree/release-1.6.0) (untested with other versions)
* [CMake](http://www.cmake.org/) v3.4.3 (or newer)

### Linux Requirements ###

These are the base requirements to build and use RapidAssist:

  * GNU-compatible Make or gmake
  * POSIX-standard shell
  * A C++98-standard-compliant compiler

### Windows Requirements ###

* Microsoft Visual C++ 2010 or newer



## Build steps ##

The following steps show how to build the library:

1) Define the environment variable `RAPIDASSIST_HOME` to the location where the source code was extracted.

2) Define the environment variable `GOOGLETEST_HOME` such that `GOOGLETEST_HOME=$RAPIDASSIST_HOME/lib/googletest`.

3) Configure the _Visual Studio solution_ or the _Makefile_ using the following commands:

   * cd $RAPIDASSIST_HOME
   * mkdir build
   * cd build
   * cmake ..

4) Build the source code:
   1) On Windows, run `cmake --build . --config Release` or open `RapidAssist.sln` with Visual Studio.
   2) On Linux, run `make` command.



### Build options ###

The library support the `RAPIDASSIST_BUILD_TEST` build option. This boolean flag is used to control generation of the library unit tests targets. The flag is set to `OFF` by default.

To enable the generation of unit test targets, run the following command at the cmake configuration time:
```cmake
cmake -DRAPIDASSIST_BUILD_TEST=ON ..
```



# Testing #

RapidAssist comes with unit tests which help maintaining the product stability and level of quality.

Test are build using the Google Test v1.6.0 framework. For more information on how googletest is working, see the [google test documentation primer](https://github.com/google/googletest/blob/release-1.8.0/googletest/docs/V1_6_Primer.md).  

Test are automatically build when building the solution.

To run tests, browse to the `build/bin` folder and run `rapidassist_unittest` executable. Windows users must also specify the configuration name after the `bin` folder (for example `build\bin\Release`).

Test results are saved in junit format in file `rapidassist_unittest.x86.debug.xml` or `rapidassist_unittest.x86.release.xml` depending on the selected configuration.

The latest test results are available at the beginning of the [README.md](README.md) file.

