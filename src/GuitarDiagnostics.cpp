#include "App/Application.h"

#include <Logger.h>

#include <exception>
#include <memory>

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    Kappa::Logger::SetLoggerName("GuitarDiagnostics");
    LOG_INFO("Guitar Diagnostic Analyzer - Starting...");

    try
    {
        auto app = std::make_unique<GuitarDiagnostics::App::Application>();
        app->Run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Fatal error: {}", e.what());
        return 1;
    }

    LOG_INFO("Guitar Diagnostic Analyzer - Exiting");
    return 0;
}
