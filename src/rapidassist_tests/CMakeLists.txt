include_directories(${CMAKE_SOURCE_DIR}/src/rapidassist)
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

add_executable(rapidassist_tests
  main.cpp
  TestEnvironmentFunc.cpp
  TestEnvironmentFunc.h
  TestFilesystemFunc.cpp
  TestFilesystemFunc.h
  TestNativeFunc.cpp
  TestNativeFunc.h
  TestStringFunc.cpp
  TestStringFunc.h
)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(rapidassist_tests debug     rapidassist gtest)
target_link_libraries(rapidassist_tests optimized rapidassist gtest)
