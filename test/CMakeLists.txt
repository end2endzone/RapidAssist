file(GLOB_RECURSE headers ${CMAKE_SOURCE_DIR}/include/*.h)

find_package(GTest)

add_executable(rapidassist_unittest
  ${headers}
  ${RAPIDASSIST_EXPORT_HEADER}
  ${RAPIDASSIST_VERSION_HEADER}
  ${RAPIDASSIST_CONFIG_HEADER}
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

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(rapidassist_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(rapidassist_unittest PUBLIC $<BUILD_INTERFACE:${GTEST_INCLUDE_DIR}>)
add_dependencies(rapidassist_unittest rapidassist)
target_link_libraries(rapidassist_unittest PUBLIC rapidassist ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} )

install(TARGETS rapidassist_unittest
        EXPORT rapidassist-targets
        ARCHIVE DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${RAPIDASSIST_INSTALL_BIN_DIR}
)
