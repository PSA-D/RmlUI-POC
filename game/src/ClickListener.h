#pragma once

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/EventListener.h>

namespace poc
{
    // Telt klikken op een element en toont het aantal in een label. Bewijst
    // dat events werken en dat de UI dynamisch aangepast kan worden.
    class ClickListener final : public Rml::EventListener
    {
    public:
        explicit ClickListener(Rml::Element* labelElement);

        void ProcessEvent(Rml::Event& event) override;

        [[nodiscard]] auto getClickCount() const -> int;

    private:
        // Geen eigenaar: het element wordt beheerd door RmlUi (de Context
        // bezit het document en dus ook zijn elementen).
        Rml::Element* label;
        int clickCount = 0;
    };
} // namespace poc
