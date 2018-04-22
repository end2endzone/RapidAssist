include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

add_library(rapidassist STATIC 
  ${PROJECT_BINARY_DIR}/src/rapidassist/version.h
  cppencoder.cpp
  cppencoder.h
  environment.cpp
  environment.h
  filesystem.cpp
  filesystem.h
  gtesthelper.cpp
  gtesthelper.h
  random.cpp
  random.h
  time_.cpp
  time_.h
  string_.cpp
  string_.h
)

link_directories(${LIBRARY_OUTPUT_PATH})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
