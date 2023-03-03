# cmake-format: off

#
# Project settings
#

option(${PROJECT_NAME}_USE_ALT_NAMES "Use alternative names for the project, such as naming the include directory all lowercase." ON)

#
# Compiler options
#
option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors." OFF)

#
# Unit testing
#
# Currently supporting: GoogleTest.
option(${PROJECT_NAME}_ENABLE_UNIT_TESTING "Enable unit tests for the projects (from the `test` subfolder)." ON)

option(${PROJECT_NAME}_USE_GTEST "Use the GoogleTest project for creating unit tests." ON)
option(${PROJECT_NAME}_USE_GOOGLE_MOCK "Use the GoogleMock project for extending the unit tests." OFF)

#
# Static analyzers
#
# Currently supporting: Clang-Tidy, Cppcheck.
option(${PROJECT_NAME}_ENABLE_CLANG_TIDY "Enable static analysis with Clang-Tidy." OFF)
option(${PROJECT_NAME}_ENABLE_CPPCHECK "Enable static analysis with Cppcheck." OFF)

#
# Code coverage
#
option(${PROJECT_NAME}_ENABLE_CODE_COVERAGE "Enable code coverage through GCC." OFF)

#
# Miscellaneous options
#

# Generate compile_commands.json for clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(${PROJECT_NAME}_VERBOSE_OUTPUT "Enable verbose output, allowing for a better understanding of each step taken." OFF)
option(${PROJECT_NAME}_GENERATE_EXPORT_HEADER "Create a `project_export.h` file containing all exported symbols." OFF)

option(${PROJECT_NAME}_ENABLE_CCACHE "Enable the usage of Ccache, in order to speed up rebuild times." ON)
find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# cmake-format: on
