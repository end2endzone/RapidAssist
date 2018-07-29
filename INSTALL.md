# Install #

The library does not provides pre-build binaries or provides an installation package. To install the binary files on the system, the source code must be compiled and copied to the appropriate directory.

The following steps show how to install the library:

1) Download the source code from an existing [tags](http://github.com/end2endzone/RapidAssist/tags) and extract the content to a local directory (for example `c:\projects\RapidAssist` or `~/dev/RapidAssist`).

2) Build the source code according to the [Build Steps](#build-steps) instructions specified in this document.

3) Navigate to the `build` directory and execute the 'install' command for your platform:
   1) On Windows, enter `cmake --build . --config Release --target INSTALL`.
   2) On Linux, enter `sudo make install`.




# Build #

This section explains how to compile and build the software and how to get a development environment ready.




## Prerequisites ##


### Software Requirements ###
The following software must be installed on the system for compiling source code:

* [Google C++ Testing Framework v1.8.0](https://github.com/google/googletest/tree/release-1.8.0)
* [CMake](http://www.cmake.org/) v3.4.3 (or newer)



### Linux Requirements ###

These are the base requirements to build and use RapidAssist:

  * GNU-compatible Make or gmake
  * POSIX-standard shell
  * A C++98-standard-compliant compiler



### Windows Requirements ###

* Microsoft Visual C++ 2010 or newer




## Build steps ##

The RapidAssist library uses the CMake build system to generate a platform-specific build environment. CMake reads the CMakeLists.txt files that you'll find throughout the directories, checks for installed dependencies and then generates files for the selected build system.

To build the software, execute the following steps:

1) Download the source code from an existing [tags](http://github.com/end2endzone/RapidAssist/tags) and extract the content to a local directory (for example `c:\projects\RapidAssist` or `~/dev/RapidAssist`).

2) Generate the project files for your build system using the following commands:
```
mkdir build
cd build
cmake ..
```

3) Build the source code:
   1) On Windows, run `cmake --build . --config Release` or open `RapidAssist.sln` with Visual Studio.
   2) On Linux, run `make` command.


## Build options ##

The following table shows the available build option supported:

| Name | Type | Default | Usage |
|------|------|:-------:|-------|
| CMAKE_INSTALL_PREFIX   | STRING | See CMake documentation | Defines the installation folder of the library.           |
| BUILD_SHARED_LIBS      | BOOL   | OFF                     | Enable/disable the generation of shared library makefiles |
| RAPIDASSIST_BUILD_TEST | BOOL   | OFF                     | Enable/disable the generation of unit tests target. |
| RAPIDASSIST_BUILD_DOC  | BOOL   | OFF                     | Enable/disable the generation of API documentation target. |

To enable a build option, run the following command at the cmake configuration time:
```cmake
cmake -D<BUILD-OPTION-NAME>=ON ..
```



# Testing #
RapidAssist comes with unit tests which help maintaining the product stability and level of quality.

Test are build using the Google Test v1.8.0 framework. For more information on how googletest is working, see the [google test documentation primer](https://github.com/google/googletest/blob/release-1.8.0/googletest/docs/V1_6_Primer.md).  

Test are disabled by default and must be manually enabled. See the [Build Options](#build-options) for details on activating unit tests.

Test are automatically build when building the solution.

To run tests, navigate to the `build/bin` folder and run `rapidassist_unittest` executable. For Windows users, the executable is located in `build\bin\Release`.

Test results are saved in junit format in file `rapidassist_unittest.x86.debug.xml` or `rapidassist_unittest.x86.release.xml` depending on the selected configuration.

The latest test results are available at the beginning of the [README.md](README.md) file.
