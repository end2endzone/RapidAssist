file(GLOB_RECURSE headers ${CMAKE_SOURCE_DIR}/include/*.h)

find_package(GTest)

add_library(rapidassist STATIC
  ${headers}
  ${RAPIDASSIST_EXPORT_HEADER}
  ${RAPIDASSIST_VERSION_HEADER}
  ${RAPIDASSIST_CONFIG_HEADER}
  cli.cpp
  cppencoder.cpp
  environment.cpp
  filesystem.cpp
  gtesthelp.cpp
  logger.cpp
  random.cpp
  time_.cpp
  strings.cpp
)

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(rapidassist PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(rapidassist PUBLIC
  $<BUILD_INTERFACE:${GTEST_INCLUDE_DIR}>                 # for GTest
  $<BUILD_INTERFACE:${RAPIDASSIST_INCLUDE_DIR}/include>   # for headers when building
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>                  # for ${RAPIDASSIST_EXPORT_HEADER} when building
  $<INSTALL_INTERFACE:${RAPIDASSIST_INSTALL_INCLUDE_DIR}> # for client in install mode
  $<INSTALL_INTERFACE:${RAPIDASSIST_INSTALL_LIB_DIR}>     # for ${RAPIDASSIST_EXPORT_HEADER} in install mode)
)
target_link_libraries(rapidassist PUBLIC ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} )

install(TARGETS rapidassist
        EXPORT rapidassist-targets
        ARCHIVE DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${RAPIDASSIST_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${RAPIDASSIST_INSTALL_BIN_DIR}
)
