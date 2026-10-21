#pragma once

#include <RmlUi/Core/SystemInterface.h>

#include <chrono>

namespace poc
{
    // Levert RmlUi de verstreken tijd en toont de logmeldingen van RmlUi in de
    // console. Zonder dit falen dingen als een ontbrekend lettertype stil.
    class LoggingSystemInterface final : public Rml::SystemInterface
    {
    public:
        LoggingSystemInterface();

        double GetElapsedTime() override;
        bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

    private:
        std::chrono::steady_clock::time_point startTime;
    };
} // namespace poc
