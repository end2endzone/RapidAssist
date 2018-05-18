include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

file(GLOB_RECURSE RAPIDASSIST_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

SET(RAPIDASSIST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/rapidassist)

add_library(rapidassist STATIC 
  ${PROJECT_BINARY_DIR}/src/rapidassist/version.h
  ${RAPIDASSIST_HEADERS}
  cli.cpp
  cppencoder.cpp
  environment.cpp
  filesystem.cpp
  gtesthelp.cpp
  logger.cpp
  random.cpp
  time_.cpp
  strings.cpp
)

link_directories(${LIBRARY_OUTPUT_PATH})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
