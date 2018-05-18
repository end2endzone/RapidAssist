include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories($ENV{GOOGLETEST_HOME}/include)

add_executable(rapidassist_unittest
  main.cpp
  TestCli.cpp
  TestCli.h
  TestDemo.cpp
  TestDemo.h
  TestEnvironment.cpp
  TestEnvironment.h
  TestFilesystem.cpp
  TestFilesystem.h
  TestGTestHelp.cpp
  TestGTestHelp.h
  TestLogger.cpp
  TestLogger.h
  TestRandom.cpp
  TestRandom.h
  TestString.cpp
  TestString.h
  TestTime.cpp
  TestTime.h
)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(rapidassist_unittest debug     rapidassist gtest)
target_link_libraries(rapidassist_unittest optimized rapidassist gtest)
