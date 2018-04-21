include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/include)
#link_directories(${LIBRARY_OUTPUT_PATH})

add_library(rapidassist STATIC 
  ${PROJECT_BINARY_DIR}/src/rapidassist/version.h
  cppencoder.cpp
  cppencoder.h
  environmentfunc.cpp
  environmentfunc.h
  filesystemfunc.cpp
  filesystemfunc.h
  gtesthelper.cpp
  gtesthelper.h
  nativefunc.cpp
  nativefunc.h
  stringfunc.cpp
  stringfunc.h
)

link_directories(${LIBRARY_OUTPUT_PATH})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
