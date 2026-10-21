#include "LoggingSystemInterface.h"

#include <chrono>
#include <iostream>

namespace poc
{
    LoggingSystemInterface::LoggingSystemInterface() : startTime(std::chrono::steady_clock::now())
    {
    }

    double LoggingSystemInterface::GetElapsedTime()
    {
        const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - startTime;
        return elapsed.count();
    }

    bool LoggingSystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message)
    {
        const char* label = "INFO";
        if (type == Rml::Log::LT_ERROR)
        {
            label = "ERROR";
        }
        else if (type == Rml::Log::LT_WARNING)
        {
            label = "WARNING";
        }

        std::cerr << "[RmlUi " << label << "] " << message << '\n';

        // true = RmlUi mag doorgaan (bij een assert zou false afbreken).
        return true;
    }
} // namespace poc
