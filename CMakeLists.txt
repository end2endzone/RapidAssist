cmake_minimum_required(VERSION 3.4.3)
project(RapidAssist)

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

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

#------------------------------------------------------------------------------------------------------------
# Set a default build type if none was specified.
# See https://blog.kitware.com/cmake-and-the-default-build-type/
#------------------------------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#------------------------------------------------------------------------------------------------------------
# Define product version according to Semantic Versioning v2.0.0 https://semver.org/
#------------------------------------------------------------------------------------------------------------
MESSAGE("")
SET(RAPIDASSIST_MAJOR_VERSION 0)
SET(RAPIDASSIST_MINOR_VERSION 4)
SET(RAPIDASSIST_PATCH_VERSION 0)
SET(RAPIDASSIST_PRERELEASE_VERSION "") #i.e: rc1
SET(RAPIDASSIST_METADATA_VERSION "")   #i.e: build.45
SET(RAPIDASSIST_VERSION_STRING ${RAPIDASSIST_MAJOR_VERSION}.${RAPIDASSIST_MINOR_VERSION}.${RAPIDASSIST_PATCH_VERSION})
IF(NOT ${RAPIDASSIST_PRERELEASE_VERSION} STREQUAL "")
  SET(RAPIDASSIST_VERSION_STRING ${RAPIDASSIST_VERSION_STRING}-${RAPIDASSIST_PRERELEASE_VERSION})
ENDIF()
IF(NOT ${RAPIDASSIST_METADATA_VERSION} STREQUAL "")
  SET(RAPIDASSIST_VERSION_STRING ${RAPIDASSIST_VERSION_STRING}+${RAPIDASSIST_METADATA_VERSION})
ENDIF()
MESSAGE("RAPIDASSIST_VERSION_STRING: ${RAPIDASSIST_VERSION_STRING}")
MESSAGE("")

# Generate version files for different systems
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/src/rapidassist/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/rapidassist/version.h")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/version.in"                   "${CMAKE_CURRENT_SOURCE_DIR}/version")

#------------------------------------------------------------------------------------------------------------
# Validate mandatory environment variables.
#------------------------------------------------------------------------------------------------------------
if(NOT EXISTS "$ENV{GOOGLETEST_HOME}")
  message(FATAL_ERROR "Could not find GOOGLETEST_HOME environment variable")
endif()

#------------------------------------------------------------------------------------------------------------
# Add source directories
#------------------------------------------------------------------------------------------------------------
add_subdirectory(src/rapidassist)

#------------------------------------------------------------------------------------------------------------
# unit tests
#------------------------------------------------------------------------------------------------------------
option(RAPIDASSIST_BUILD_TEST "Build all RapidAssist unit tests" OFF)
if(RAPIDASSIST_BUILD_TEST)
  add_subdirectory(test)

  # Add googletest to project
  if (NOT TARGET gtest)
    if (WIN32)
      # https://stackoverflow.com/a/12546288
      # Force building googletest in `Multi-threaded Debug DLL (/MDd)` and `Multi-threaded DLL (/MD)`
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory($ENV{GOOGLETEST_HOME})
    set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
  endif()

  # Define project dependencies
  add_dependencies(rapidassist_unittest rapidassist)
  add_dependencies(rapidassist_unittest gtest)
endif()

#------------------------------------------------------------------------------------------------------------
# Generate doxygen documentation
# See https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
#------------------------------------------------------------------------------------------------------------
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
