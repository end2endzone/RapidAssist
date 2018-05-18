include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories($ENV{GOOGLETEST_HOME}/include)

file(GLOB_RECURSE RAPIDASSIST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(rapidassist STATIC 
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

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
