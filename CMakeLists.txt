cmake_minimum_required(VERSION 3.15)

#
# Project details
#
project(
    "nama"
    VERSION 0.1.0
    LANGUAGES CXX
)

#
# Set project options
#
include(cmake/StandardSettings.cmake)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Utils.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -Og")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

message(STATUS "${CMAKE_BUILD_TYPE} Build")

message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")

if(UNIX)
    # this will allow to use same _DEBUG macro available in both Linux as well as Windows - MSCV environment. Easy to
    # put Debug specific code.
    add_compile_options("$<$<CONFIG:DEBUG>:-D_DEBUG>")
endif(UNIX)

#
# Setup alternative names
#
if(${PROJECT_NAME}_USE_ALT_NAMES)
    string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
    string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)
else()
    set(PROJECT_NAME_LOWERCASE ${PROJECT_NAME})
    set(PROJECT_NAME_UPPERCASE ${PROJECT_NAME})
endif()

#
# Prevent building in the source directory
#
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
        FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n"
    )
endif()

#
# Create library, setup header and source files
#

include(cmake/Macros.cmake)
create_header(${PROJECT_NAME})

# Find all headers and implementation files
include(cmake/SourcesAndHeaders.cmake)

add_library(${PROJECT_NAME} ${headers} ${sources})

if(${PROJECT_NAME}_VERBOSE_OUTPUT)
    verbose_message("Found the following sources:")

    foreach(source IN LISTS sources)
        verbose_message("* ${source}")
    endforeach()

    verbose_message("Found the following headers:")

    foreach(header IN LISTS headers)
        verbose_message("* ${header}")
    endforeach()
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)

message(STATUS "Added all header and implementation files.\n")

#
# Set the project standard and warnings
#
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

verbose_message("Applied compiler warnings. Using standard ${CMAKE_CXX_STANDARD}.\n")

#
# Dependencies
#

# cmake-format: off
# Identify and link with the specific "packages" the project uses
# find_package(package_name package_version REQUIRED package_type [other_options])
# target_link_libraries(
#  ${PROJECT_NAME}
#  PUBLIC
#    dependency1 ...
#  PRIVATE
#    dependency2 ...
#    ${PROJECT_NAME}_PROJECT_OPTIONS
#    ${PROJECT_NAME}_PROJECT_WARNINGS
#)
# cmake-format: on

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)
add_subdirectory(external/stb)
add_subdirectory(external/tinyobjloader)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES} glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen OpenMP::OpenMP_CXX)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/shiokaze)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/stb/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/tinyobjloader/include)
target_link_libraries(${PROJECT_NAME} PRIVATE stb tinyobjloader)

verbose_message("Successfully added all dependencies and linked against them.")

#
# Set the build/user include directories
#

# Allow usage of header files in the `src` directory, but only for utilities
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

message(STATUS "Finished setting up include directories.")

#
# Provide alias to library for
#
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

verbose_message("Project is now aliased as ${PROJECT_NAME}::${PROJECT_NAME}.\n")

#
# Unit testing setup
#
if(${PROJECT_NAME}_ENABLE_UNIT_TESTING)
    enable_testing()
    message(STATUS "Build unit tests for the project. Tests should always be found in the test folder\n")
    add_subdirectory(test)
endif()

#
# Application setup
#

add_subdirectory(apps)
