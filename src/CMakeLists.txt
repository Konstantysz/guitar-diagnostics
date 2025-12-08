add_library(GuitarDiagnosticsCore STATIC
    # Analysis
    Analysis/AnalysisResult.cpp

    # Application layer
    # App/Application.cpp
    App/AudioProcessingLayer.cpp
    # App/DiagnosticVisualizationLayer.cpp

    # Audio management
    Audio/AudioDeviceManager.cpp

    # Analysis engine
    Analysis/AnalysisEngine.cpp

    # Analyzers
    Analysis/Fretbuzz/FretBuzzDetector.cpp
    Analysis/Intonation/IntonationAnalyzer.cpp
    Analysis/StringHealth/StringHealthAnalyzer.cpp

    # UI
    # UI/TabController.cpp
    # UI/Panels/FretBuzzPanel.cpp
    # UI/Panels/IntonationPanel.cpp
    # UI/Panels/StringHealthPanel.cpp
    # UI/Panels/AudioMonitorPanel.cpp

    # Utilities
    # Util/SignalGenerator.cpp
)

# Create alias for consistent naming
add_library(GuitarDiagnostics::Core ALIAS GuitarDiagnosticsCore)

# Include directories
target_include_directories(GuitarDiagnosticsCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(GuitarDiagnosticsCore
    PUBLIC
        Kappa
        guitar-io
        guitar-dsp

)

# C++20 features
target_compile_features(GuitarDiagnosticsCore PUBLIC cxx_std_20)

# Compiler warnings (only for our code, not third-party)
if(GD_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(GuitarDiagnosticsCore PRIVATE /W4 /WX)
    else()
        target_compile_options(GuitarDiagnosticsCore PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# AddressSanitizer (only for our code)
if(GD_ENABLE_ASAN)
    if(MSVC)
        target_compile_options(GuitarDiagnosticsCore PRIVATE /fsanitize=address)
        target_link_options(GuitarDiagnosticsCore PRIVATE /fsanitize=address)
    else()
        target_compile_options(GuitarDiagnosticsCore PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(GuitarDiagnosticsCore PRIVATE -fsanitize=address)
    endif()
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(GuitarDiagnosticsCore PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()
