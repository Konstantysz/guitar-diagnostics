cmake_minimum_required(VERSION 3.20)
project(GuitarDiagnostics VERSION 0.0.1 LANGUAGES CXX)

# C++20 standard (required)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(GD_BUILD_TESTS "Build unit tests" ON)
option(GD_ENABLE_ASAN "Enable AddressSanitizer for debugging" OFF)
option(GD_ENABLE_WARNINGS "Enable strict compiler warnings" ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(STATUS "Updating git submodules...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
    endif()
endif()

# Verify submodules exist
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/kappa-core/CMakeLists.txt")
    message(FATAL_ERROR "kappa-core submodule not found. Run: git submodule update --init --recursive")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/lib-guitar-io/CMakeLists.txt")
    message(FATAL_ERROR "lib-guitar-io submodule not found. Run: git submodule update --init --recursive")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/lib-guitar-dsp/CMakeLists.txt")
    message(FATAL_ERROR "lib-guitar-dsp submodule not found. Run: git submodule update --init --recursive")
endif()

# Note: Compiler warnings and sanitizers are applied per-target in src/CMakeLists.txt
# to avoid affecting third-party dependencies

# Add external dependencies
add_subdirectory(external/kappa-core)
add_subdirectory(external/lib-guitar-io)
add_subdirectory(external/lib-guitar-dsp)

# Main library (all analysis logic)
add_subdirectory(src)

# Executable
add_executable(GuitarDiagnostics
    src/GuitarDiagnostics.cpp
)

target_link_libraries(GuitarDiagnostics PRIVATE
    GuitarDiagnostics::Core
)

# Apply strict warnings to main executable
if(GD_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(GuitarDiagnostics PRIVATE /W4 /WX)
    else()
        target_compile_options(GuitarDiagnostics PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# Tests
if(GD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS GuitarDiagnostics
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# CPack configuration for deployment
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "GuitarDiagnostics")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Konstantysz")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional C++20 real-time guitar diagnostics application")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "Guitar Diagnostic Analyzer Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Tests: ${GD_BUILD_TESTS}")
message(STATUS "  AddressSanitizer: ${GD_ENABLE_ASAN}")
message(STATUS "  Strict Warnings: ${GD_ENABLE_WARNINGS}")
message(STATUS "")
