include_directories(${CMAKE_SOURCE_DIR}/src/rapidassist)
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

add_executable(rapidassist_unittest
  main.cpp
  TestCli.cpp
  TestCli.h
  TestEnvironment.cpp
  TestEnvironment.h
  TestFilesystem.cpp
  TestFilesystem.h
  TestGTestHelper.cpp
  TestGTestHelper.h
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
