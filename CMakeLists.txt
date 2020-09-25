cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("testing" CXX)

if(NOT TARGET testing)

    set (TESTING_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    set (TESTING_CXX_FLAGS "-std=c++11")

    get_directory_property(hasParent PARENT_DIRECTORY)
    if (hasParent)
      set (TESTING_INCLUDE_DIRS ${TESTING_INCLUDE_DIRS} PARENT_SCOPE)
      set (TESTING_CXX_FLAGS ${TESTING_CXX_FLAGS} PARENT_SCOPE)
    endif()

    message(STATUS "TESTING_CXX_FLAGS: ${TESTING_CXX_FLAGS}")

    set (TESTING_VERSION_MAJOR 1)
    set (TESTING_VERSION_MINOR 0)
    set (TESTING_VERSION_PATCH 0)
    set (TESTING_VERSION "${TESTING_VERSION_MAJOR}.${TESTING_VERSION_MINOR}.${TESTING_VERSION_PATCH}")

    set (TESTING_INCLUDE_INSTALL_DIR include/testing)
    set (TESTING_LIBRARY_INSTALL_DIR lib)

if (APPLE)
    add_definitions(-DCOCOA)
elseif (UNIX)
    add_definitions(-DX11)
elseif (WINDOWS)
    add_definitions(-DWIN32)
endif ()

if (BUILD_FOR_ARM)
    add_definitions(-DBUILD_FOR_ARM -Wno-psabi)
endif ()

    include_directories(${TESTING_INCLUDE_DIRS})
    add_definitions(${TESTING_CXX_FLAGS})

    # header file to pass version settings to the source code
    configure_file (
        "${PROJECT_SOURCE_DIR}/version.h.in"
        "${PROJECT_BINARY_DIR}/testing_version.h"
    )

    set(SOURCE_FILES testing.cpp)
    set(INCLUDE_FILES testing.h)
    set(CMAKE_DEBUG_POSTFIX d)

    add_library(testing ${SOURCE_FILES} ${INCLUDE_FILES})
    set_target_properties(testing PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          FOLDER libraries)

    install(TARGETS testing DESTINATION ${TESTING_LIBRARY_INSTALL_DIR})

    install(FILES ${INCLUDE_FILES}
                  ${PROJECT_BINARY_DIR}/testing_version.h
            DESTINATION ${TESTING_INCLUDE_INSTALL_DIR})

endif()
