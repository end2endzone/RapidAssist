# CMakeLists.txt
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
project(RapidAssist)

find_package(GTest)

if (GTEST_FOUND)
  set(RAPIDASSIST_HAVE_GTEST 1)
else()
  set(RAPIDASSIST_HAVE_GTEST)
  set(GTEST_INCLUDE_DIR "")
  set(GTEST_LIBRARIES "")
endif()

##############################################################################################################################################
# Standard CMake variables
##############################################################################################################################################

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# Set a default build type if none was specified.
# See https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Export no symbols by default (if the compiler supports it). 
# This makes e.g. GCC's "visibility behavior" consistent with MSVC's.  
# On Windows/MSVC this is a noop. 
if (BUILD_SHARED_LIBS)
  set(CMAKE_C_VISIBILITY_PRESET hidden) 
  set(CMAKE_CXX_VISIBILITY_PRESET hidden) 
endif()

# Set the output directory where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

##############################################################################################################################################
# CMake properties
##############################################################################################################################################
MESSAGE( STATUS "PROJECT_NAME:             " ${PROJECT_NAME} )
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "PROJECT_BINARY_DIR:       " ${PROJECT_BINARY_DIR} )
MESSAGE( STATUS "PROJECT_SOURCE_DIR:       " ${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH:   " ${EXECUTABLE_OUTPUT_PATH} )
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:      " ${LIBRARY_OUTPUT_PATH} )
MESSAGE( STATUS "CMAKE_MODULE_PATH:        " ${CMAKE_MODULE_PATH} )
MESSAGE( STATUS "CMAKE_COMMAND:            " ${CMAKE_COMMAND} )
MESSAGE( STATUS "CMAKE_ROOT:               " ${CMAKE_ROOT} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE:  " ${CMAKE_CURRENT_LIST_FILE} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE:  " ${CMAKE_CURRENT_LIST_LINE} )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH:       " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH:       " ${CMAKE_LIBRARY_PATH} )
MESSAGE( STATUS "CMAKE_SYSTEM:             " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME:        " ${CMAKE_SYSTEM_NAME} )
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION:     " ${CMAKE_SYSTEM_VERSION} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR:   " ${CMAKE_SYSTEM_PROCESSOR} )

##############################################################################################################################################
# Global settings
##############################################################################################################################################

# Product version according to Semantic Versioning v2.0.0 https://semver.org/
set(RAPIDASSIST_VERSION_MAJOR 0)
set(RAPIDASSIST_VERSION_MINOR 8)
set(RAPIDASSIST_VERSION_PATCH 0)
set(RAPIDASSIST_VERSION ${RAPIDASSIST_VERSION_MAJOR}.${RAPIDASSIST_VERSION_MINOR}.${RAPIDASSIST_VERSION_PATCH})

# read license file
file(READ ${CMAKE_SOURCE_DIR}/LICENSE.h LICENSE)

# version.h file
set(RAPIDASSIST_VERSION_HEADER ${CMAKE_BINARY_DIR}/include/rapidassist/version.h)
message("Generating ${RAPIDASSIST_VERSION_HEADER}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/rapidassist/version.h.in ${RAPIDASSIST_VERSION_HEADER} )

# config.h file
set(RAPIDASSIST_CONFIG_HEADER ${CMAKE_BINARY_DIR}/include/rapidassist/config.h)
message("Generating ${RAPIDASSIST_CONFIG_HEADER}...")
if (BUILD_SHARED_LIBS)
  set(RAPIDASSIST_BUILT_AS_SHARED 1)
else()
  set(RAPIDASSIST_BUILT_AS_STATIC 1)
endif()
configure_file( ${CMAKE_SOURCE_DIR}/src/rapidassist/config.h.in ${RAPIDASSIST_CONFIG_HEADER} )

# Define installation directories
set(RAPIDASSIST_INSTALL_BIN_DIR      "bin")
set(RAPIDASSIST_INSTALL_LIB_DIR      "lib/rapidassist-${RAPIDASSIST_VERSION}")
set(RAPIDASSIST_INSTALL_INCLUDE_DIR  "include/rapidassist-${RAPIDASSIST_VERSION}")
set(RAPIDASSIST_INSTALL_CMAKE_DIR    ${RAPIDASSIST_INSTALL_LIB_DIR}) # CMake files (*.cmake) should have the same destination as the library files. Some also prefers to use "cmake".

##############################################################################################################################################
# Project settings
##############################################################################################################################################

# Build options
option(RAPIDASSIST_BUILD_TEST "Build all RapidAssist's unit tests" OFF)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Prevents annoying warnings on MSVC
if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Define include directories for source code.
# The specified values will not be exported.
set( RAPIDASSIST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include )
include_directories(${RAPIDASSIST_INCLUDE_DIR}                # public header files, for source code.
                    ${CMAKE_BINARY_DIR}/include               # for ${RAPIDASSIST_VERSION_HEADER} and ${RAPIDASSIST_CONFIG_HEADER} generated files.
)

# Subprojects
add_subdirectory(src/rapidassist)

if(RAPIDASSIST_BUILD_TEST)
  if (GTEST_FOUND)
    add_subdirectory(test)
  else()
    message(WARNING "RAPIDASSIST_BUILD_TEST is enabled but gtest library is not found. Unit tests wont be added to the project.")
  endif()
endif()

##############################################################################################################################################
# Support for static and shared library
##############################################################################################################################################

if (BUILD_SHARED_LIBS)
  set(RAPIDASSIST_EXPORT_HEADER_FILENAME "export.h")
  set(RAPIDASSIST_EXPORT_HEADER ${CMAKE_BINARY_DIR}/include/rapidassist/${RAPIDASSIST_EXPORT_HEADER_FILENAME})
  message("Generating ${RAPIDASSIST_EXPORT_HEADER_FILENAME} for shared library...")
  include (GenerateExportHeader) 
  GENERATE_EXPORT_HEADER(rapidassist 
               BASE_NAME rapidassist 
               EXPORT_MACRO_NAME RAPIDASSIST_EXPORT 
               EXPORT_FILE_NAME ${RAPIDASSIST_EXPORT_HEADER} 
               STATIC_DEFINE RAPIDASSIST_BUILT_AS_STATIC
  )
endif()

##############################################################################################################################################
# Generate doxygen documentation
# See https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
##############################################################################################################################################
option(RAPIDASSIST_BUILD_DOC "Build RapidAssist documentation" OFF)
if (RAPIDASSIST_BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
 
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
 
    # note the option ALL which allows to build the docs together with the application
    add_custom_target( rapidassist_doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif()

##############################################################################################################################################
# Install
##############################################################################################################################################

# Install locations:   See https://unix.stackexchange.com/a/36874
#   On UNIX, installs to "/usr/local".
#   On Windows, installs to "C:\Program Files (x86)\${PROJECT_NAME}" or to "C:\Program Files\${PROJECT_NAME}" for 64 bit binaries

# Target config version verification file
configure_file(${CMAKE_SOURCE_DIR}/cmake/rapidassist-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/rapidassist-config-version.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/rapidassist-config-version.cmake DESTINATION ${RAPIDASSIST_INSTALL_CMAKE_DIR})

# Target config file
configure_file(${CMAKE_SOURCE_DIR}/cmake/rapidassist-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/rapidassist-config.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/rapidassist-config.cmake DESTINATION ${RAPIDASSIST_INSTALL_CMAKE_DIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/rapidassist DESTINATION ${RAPIDASSIST_INSTALL_INCLUDE_DIR})
install(FILES ${RAPIDASSIST_EXPORT_HEADER}
              ${RAPIDASSIST_VERSION_HEADER}
              ${RAPIDASSIST_CONFIG_HEADER}
              DESTINATION ${RAPIDASSIST_INSTALL_INCLUDE_DIR}/rapidassist)
install(EXPORT rapidassist-targets DESTINATION ${RAPIDASSIST_INSTALL_CMAKE_DIR})

##############################################################################################################################################
# Packaging
##############################################################################################################################################

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${RAPIDASSIST_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR "${RAPIDASSIST_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${RAPIDASSIST_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${RAPIDASSIST_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RapidAssist - RapidAssist is a lite cross-platform library that assist you with the most c++ repetitive tasks.")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
