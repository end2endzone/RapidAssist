include_directories(${CMAKE_SOURCE_DIR}/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

include_directories($ENV{GOOGLETEST_HOME}/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

file(GLOB_RECURSE RAPIDASSIST_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

SET(RAPIDASSIST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/rapidassist)

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

link_directories(${LIBRARY_OUTPUT_PATH})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
