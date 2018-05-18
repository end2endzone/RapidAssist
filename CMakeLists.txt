cmake_minimum_required(VERSION 3.4.3)
project(RapidAssist)

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

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

# Define product version according to Semantic Versioning v2.0.0 https://semver.org/
MESSAGE("")
SET(RAPIDASSIST_MAJOR_VERSION 0)
SET(RAPIDASSIST_MINOR_VERSION 3)
SET(RAPIDASSIST_PATCH_VERSION 1)
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
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/src/rapidassist/version.h.in"   "${PROJECT_BINARY_DIR}/src/rapidassist/version.h")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/version.in"                     "${CMAKE_CURRENT_SOURCE_DIR}/version")

# Add source directories
add_subdirectory(src/rapidassist)
add_subdirectory(src/rapidassist_unittest)

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
