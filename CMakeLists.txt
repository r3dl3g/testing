cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project("testing" LANGUAGES CXX VERSION 1.3.1)

include (GenerateExportHeader)
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

if(NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

set(TESTING_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(TESTING_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(TESTING_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(TESTING_VERSION "${PROJECT_VERSION}")

option(TESTING_CONFIG_INSTALL "On to make an installable standalone build, Off to build as part of a project. Default Off" OFF)
option(TESTING_CXX_STANDARD "C++ standard to overwrite default cmake standard" ${CMAKE_CXX_STANDARD})

function(DebugPrint MSG)
  if(VERBOSE)
    message(STATUS "${MSG}")
  endif()
endfunction(DebugPrint)

if(NOT TARGET testing)

  set(TESTING_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  if(APPLE)
    add_definitions(-DCOCOA)
  elseif(UNIX)
    add_definitions(-DX11)
  elseif(WINDOWS)
    add_definitions(-DWIN32)
  endif()

  if(TESTING_CXX_STANDARD STREQUAL "OFF")
    set(TESTING_CXX_STANDARD 17)
  endif()

  DebugPrint("TESTING_CXX_FLAGS: ${TESTING_CXX_FLAGS}")
  DebugPrint("TESTING_CXX_STANDARD: ${TESTING_CXX_STANDARD}")

  if(BUILD_FOR_ARM)
    add_definitions(-DBUILD_FOR_ARM -Wno-psabi)
  endif()

  get_directory_property(hasParent PARENT_DIRECTORY)
  if(hasParent)
    set(TESTING_INCLUDE_DIRS ${TESTING_INCLUDE_DIRS} PARENT_SCOPE)
    set(TESTING_CXX_FLAGS ${TESTING_CXX_FLAGS} PARENT_SCOPE)
    set(TESTING_CXX_STANDARD ${TESTING_CXX_STANDARD} PARENT_SCOPE)
  else()
    set(TESTING_CONFIG_INSTALL On)
  endif()

  include_directories(${TESTING_INCLUDE_DIRS})
  add_definitions(${TESTING_CXX_FLAGS})

  # header file to pass version settings to the source code
  configure_file (
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${PROJECT_BINARY_DIR}/testing/testing_version.h"
    )

  set(SOURCE_FILES testing.cpp)
  set(INCLUDE_FILES testing.h)

  if(NOT ANDROID)
    set(CMAKE_DEBUG_POSTFIX d)
  endif()

  add_library(testing STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
  add_library(testing::testing ALIAS testing)

  set_target_properties(testing PROPERTIES
                        POSITION_INDEPENDENT_CODE ON
                        FOLDER libraries
                        VERSION ${TESTING_VERSION}
                        SOVERSION ${TESTING_VERSION}
                        CXX_STANDARD ${TESTING_CXX_STANDARD})

  if(TESTING_CONFIG_INSTALL)
    set(TESTING_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/testing)
    set(TESTING_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}/cmake/testing)

    target_include_directories(testing
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${TESTING_INSTALL_INCLUDEDIR}>
    )

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/testingConfigVersion.cmake
        VERSION ${TESTING_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/testingConfig.cmake
        INSTALL_DESTINATION ${TESTING_INSTALL_LIBDIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    )

    install(TARGETS testing
        EXPORT testing_Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${TESTING_INSTALL_INCLUDEDIR}
    )

    install(FILES
        ${INCLUDE_FILES}
        ${PROJECT_BINARY_DIR}/testing/testing_version.h
        DESTINATION ${TESTING_INSTALL_INCLUDEDIR}
    )

    install(EXPORT testing_Targets
        FILE testingTargets.cmake
        NAMESPACE testing::
        DESTINATION ${TESTING_INSTALL_LIBDIR}
    )

    export(TARGETS testing FILE testingTargets.cmake)

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/testingConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/testingConfigVersion.cmake
        DESTINATION ${TESTING_INSTALL_LIBDIR}
    )

  endif()

endif()
