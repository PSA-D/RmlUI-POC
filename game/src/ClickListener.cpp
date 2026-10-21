#include "ClickListener.h"

#include <string>

namespace poc
{
    ClickListener::ClickListener(Rml::Element* labelElement) : label(labelElement)
    {
    }

    void ClickListener::ProcessEvent(Rml::Event& /*event*/)
    {
        ++clickCount;
        label->SetInnerRML("Clicked " + std::to_string(clickCount) + " time(s)");
    }

    auto ClickListener::getClickCount() const -> int
    {
        return clickCount;
    }
} // namespace poc
