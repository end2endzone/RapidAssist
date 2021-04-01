cmake_minimum_required(VERSION 3.0)
project(RapidAssistClient)

find_package(rapidassist 0.9.0 REQUIRED)

message(rapidassist_FOUND=        ${rapidassist_FOUND})
message(rapidassist_LIBRARY=      ${rapidassist_LIBRARY})
message(rapidassist_LIBRARIES=    ${rapidassist_LIBRARIES})
message(rapidassist_INCLUDE=      ${rapidassist_INCLUDE})
message(rapidassist_INCLUDES=     ${rapidassist_INCLUDES})
message(rapidassist_INCLUDE_DIR=  ${rapidassist_INCLUDE_DIR})
message(rapidassist_VERSION=      ${rapidassist_VERSION})
message(rapidassist_DIR=          ${rapidassist_DIR})

add_executable(rapidassistclient
  client.cpp
)

target_link_libraries(rapidassistclient rapidassist)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
