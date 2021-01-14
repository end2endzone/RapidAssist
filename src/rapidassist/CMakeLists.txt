set(RAPIDASSIST_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/include/rapidassist/cli.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/console.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/code_cpp.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/environment.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/environment_utf8.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/errors.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/filesystem.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/filesystem_utf8.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/generics.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/propertiesfile.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/logging.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/macros.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/process.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/process_utf8.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/random.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/strings.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/testing.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/testing_utf8.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/timing.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/undef_windows_macros.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/unicode.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/user.h
  ${CMAKE_SOURCE_DIR}/include/rapidassist/user_utf8.h
)

add_library(rapidassist STATIC
  ${RAPIDASSIST_HEADER_FILES}
  ${RAPIDASSIST_EXPORT_HEADER}
  ${RAPIDASSIST_VERSION_HEADER}
  ${RAPIDASSIST_CONFIG_HEADER}
  console.cpp
  cli.cpp
  code_cpp.cpp
  environment.cpp
  environment_utf8.cpp
  errors.cpp
  filesystem.cpp
  filesystem_utf8.cpp
  propertiesfile.cpp
  logging.cpp
  process.cpp
  process_utf8.cpp
  random.cpp
  strings.cpp
  testing.cpp
  testing_utf8.cpp
  timing.cpp
  unicode.cpp
  user.cpp
  user_utf8.cpp
)

# Unit test projects requires to link with pthread if also linking with gtest
if(RAPIDASSIST_BUILD_TESTING)
  if(NOT WIN32)
    set(PTHREAD_LIBRARIES -pthread)
  endif()
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(rapidassist PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(rapidassist
  PUBLIC
    $<INSTALL_INTERFACE:${RAPIDASSIST_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${GTEST_INCLUDE_DIR}
)
target_link_libraries(rapidassist PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES})

install(TARGETS rapidassist
        EXPORT rapidassist-targets
        ARCHIVE DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${RAPIDASSIST_INSTALL_BIN_DIR}
)
