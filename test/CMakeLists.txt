find_package(GTest)

if(RAPIDASSIST_BUILD_TESTING)
  set(RAPIDASSIST_TESTING_TEST_FILES TestTesting.cpp TestTesting.h)
endif()

set(UNITTEST_TEST_FILES ""
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestPropertiesFile.testLoad.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestPropertiesFile.testSave.expected.txt
)

add_executable(rapidassist_unittest
  ${RAPIDASSIST_EXPORT_HEADER}
  ${RAPIDASSIST_VERSION_HEADER}
  ${RAPIDASSIST_CONFIG_HEADER}
  ${RAPIDASSIST_TESTING_TEST_FILES}
  ${UNITTEST_TEST_FILES}
  main.cpp
  TestCli.cpp
  TestCli.h
  TestConsole.cpp
  TestConsole.h
  TestDemo.cpp
  TestDemo.h
  TestEnvironment.cpp
  TestEnvironment.h
  TestErrors.cpp
  TestErrors.h
  TestFilesystem.cpp
  TestFilesystem.h
  TestGenerics.cpp
  TestGenerics.h
  TestLogging.cpp
  TestLogging.h
  TestProcess.cpp
  TestProcess.h
  TestPropertiesFile.cpp
  TestPropertiesFile.h
  TestRandom.cpp
  TestRandom.h
  TestString.cpp
  TestString.h
  TestTiming.cpp
  TestTiming.h
  TestUnicode.cpp
  TestUnicode.h
  TestUser.cpp
  TestUser.h
)

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Group external files as filter for Visual Studio
source_group("Test Files"       FILES ${UNITTEST_TEST_FILES})

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(rapidassist_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(rapidassist_unittest PRIVATE ${GTEST_INCLUDE_DIR})
add_dependencies(rapidassist_unittest rapidassist)
target_link_libraries(rapidassist_unittest PUBLIC rapidassist PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} )

# Copy test files database to target dir
add_custom_command( TARGET rapidassist_unittest POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/test_files $<TARGET_FILE_DIR:rapidassist_unittest>/test_files
                    COMMENT "Copying test files database.")

# Copy test configuration files to build dir for local execution (from within the IDE)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS rapidassist_unittest
        EXPORT rapidassist-targets
        ARCHIVE DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${RAPIDASSIST_INSTALL_BIN_DIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_files
        DESTINATION ${RAPIDASSIST_INSTALL_BIN_DIR}
)
