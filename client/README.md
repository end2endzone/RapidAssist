# RapidAssist #
RapidAssist is a lite cross-platform library that assist you with the most c++ repetitive tasks.



# Using the library #

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



# Build #

This section explains how to compile the RapidAssist client example.

## Windows users ##
To build the software, execute the following steps:

1) Open a command prompt and navigate to the example directory.
2) Enter the following commands:

```cmake
mkdir build
cd build
set foolib_DIR=<foolib_folder>
cmake  ..
```

## Linux users ##
To build the software, execute the following steps:

1) Open a command prompt and navigate to the example directory.
2) Enter the following commands:

```cmake
mkdir build
cd build
export foolib_DIR=<foolib_folder>
cmake  ..
```
