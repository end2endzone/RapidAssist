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
  ratime.cpp
  ratime.h
  rastring.cpp
  rastring.h
)

link_directories(${LIBRARY_OUTPUT_PATH})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
