find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(GuitarDiagnosticsTests
    # Analysis tests
    # Analysis/TestFretbuzzDetector.cpp
    # Analysis/TestIntonationAnalyzer.cpp
    # Analysis/TestStringhealthAnalyzer.cpp
    Analysis/TestAnalysisEngine.cpp

    # Audio tests
    Audio/TestAudioDeviceManager.cpp

    # UI tests
    # UI/TestTabController.cpp

    # Integration tests
    # Integration/TestAnalysisPipeline.cpp

    # Utility tests
    Util/TestLockFreeRingBuffer.cpp
    # Util/TestSignalGenerator.cpp
)

target_link_libraries(GuitarDiagnosticsTests
    PRIVATE
        GuitarDiagnostics::Core
        GTest::gtest
        GTest::gtest_main
)

# Apply strict warnings to tests
if(GD_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(GuitarDiagnosticsTests PRIVATE /W4 /WX)
    else()
        target_compile_options(GuitarDiagnosticsTests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# Discover tests
include(GoogleTest)
gtest_discover_tests(GuitarDiagnosticsTests)

# Add custom test target
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS GuitarDiagnosticsTests
    COMMENT "Running all tests..."
)
