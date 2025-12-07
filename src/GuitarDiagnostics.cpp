#include <Logger.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    // Set logger name before any logging
    Kappa::Logger::SetLoggerName("GuitarDiagnostics");

    LOG_INFO("Guitar Diagnostic Analyzer - Starting...");

    // TODO: Initialize kappa-core application
    // TODO: Create audio processing layer
    // TODO: Create visualization layer
    // TODO: Run main loop

    LOG_INFO("Guitar Diagnostic Analyzer - Exiting");
    return 0;
}
