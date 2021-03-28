cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("testing" CXX)

include (GenerateExportHeader)
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

option(TESTING_STANDALONE_BUILD "On to make an installable standalone build, Off to build as part of a project. Default Off" OFF)
option(TESTING_CXX_STANDARD "C++ standard to overwrite default cmake standard" ${CMAKE_CXX_STANDARD})

function(DebugPrint MSG)
  if(VERBOSE)
    message(STATUS "${MSG}")
  endif()
endfunction(DebugPrint)

if(NOT TARGET testing)

  set (TESTING_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  set (TESTING_VERSION_MAJOR 1)
  set (TESTING_VERSION_MINOR 0)
  set (TESTING_VERSION_PATCH 0)
  set (TESTING_VERSION "${TESTING_VERSION_MAJOR}.${TESTING_VERSION_MINOR}.${TESTING_VERSION_PATCH}")

  if (APPLE)
    add_definitions(-DCOCOA)
  elseif (UNIX)
    add_definitions(-DX11)
  elseif (WINDOWS)
    add_definitions(-DWIN32)
  endif ()

  if (TESTING_CXX_STANDARD STREQUAL "OFF")
    set (TESTING_CXX_STANDARD 17)
  endif ()

  DebugPrint("TESTING_CXX_FLAGS: ${TESTING_CXX_FLAGS}")
  DebugPrint("TESTING_CXX_STANDARD: ${TESTING_CXX_STANDARD}")

  if (BUILD_FOR_ARM)
    add_definitions(-DBUILD_FOR_ARM -Wno-psabi)
  endif ()

  get_directory_property(hasParent PARENT_DIRECTORY)
  if (hasParent)
    set (TESTING_INCLUDE_DIRS ${TESTING_INCLUDE_DIRS} PARENT_SCOPE)
    set (TESTING_CXX_FLAGS ${TESTING_CXX_FLAGS} PARENT_SCOPE)
    set (TESTING_CXX_STANDARD ${TESTING_CXX_STANDARD} PARENT_SCOPE)
  else()
    set (TESTING_STANDALONE_BUILD On)
  endif()

  include_directories(${TESTING_INCLUDE_DIRS})
  add_definitions(${TESTING_CXX_FLAGS})

  # header file to pass version settings to the source code
  configure_file (
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${PROJECT_BINARY_DIR}/testing_version.h"
    )

  set(SOURCE_FILES testing.cpp)
  set(INCLUDE_FILES testing.h)

  if (NOT ANDROID)
    set(CMAKE_DEBUG_POSTFIX d)
  endif()

  add_library(testing STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
  set_target_properties(testing PROPERTIES
                        POSITION_INDEPENDENT_CODE ON
                        FOLDER libraries
                        VERSION ${TESTING_VERSION}
                        SOVERSION ${TESTING_VERSION}
                        CXX_STANDARD ${TESTING_CXX_STANDARD})

  if (TESTING_STANDALONE_BUILD)
    set (INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/testing)
    set (LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/testing)

    target_include_directories(testing
                               PUBLIC
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                               "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
    )

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/testingConfigVersion.cmake
                                     VERSION ${TESTING_VERSION}
                                     COMPATIBILITY SameMajorVersion)

    install(TARGETS testing
            EXPORT testingTargets
            LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
    )

    install(FILES ${INCLUDE_FILES}
            ${PROJECT_BINARY_DIR}/testing_version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}
    )

    install(EXPORT testingTargets
            FILE testingTargets.cmake
            NAMESPACE testing::
            DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
    )


endif()

endif()
