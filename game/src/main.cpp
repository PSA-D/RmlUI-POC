#include "ClickListener.h"
#include "LoggingSystemInterface.h"
#include "NullRenderInterface.h"

#include <RmlUi/Core.h>

#include <cstdlib>
#include <iostream>
#include <string>

#ifndef POC_ASSET_DIR
#error "POC_ASSET_DIR moet door CMake gedefinieerd worden."
#endif

namespace
{
    auto check(bool condition, const std::string& description) -> bool
    {
        std::cout << (condition ? "[PASS] " : "[FAIL] ") << description << '\n';
        return condition;
    }

    // Draait alle checks. Alle RmlUi-objecten (context, document) horen bij
    // RmlUi zelf en worden pas opgeruimd in Rml::Shutdown(), dus we geven hier
    // alleen niet-bezittende pointers rond.
    auto runChecks(poc::NullRenderInterface& renderInterface) -> bool
    {
        const std::string assetDir = POC_ASSET_DIR;
        bool allPassed = true;

        // 1. Lettertype laden. Zonder lettertype hebben tekst-elementen
        //    breedte 0 en zijn de layout-checks misleidend.
        const bool fontLoaded = Rml::LoadFontFace(assetDir + "/fonts/DejaVuSans.ttf");
        allPassed = check(fontLoaded, "Lettertype geladen") && allPassed;
        if (!fontLoaded)
        {
            return false;
        }

        // 2. Context aanmaken (de "virtuele" schermruimte voor de UI).
        Rml::Context* context = Rml::CreateContext("main", Rml::Vector2i(800, 600));
        allPassed = check(context != nullptr, "Context aangemaakt") && allPassed;
        if (context == nullptr)
        {
            return false;
        }

        // 3. Document laden en tonen.
        Rml::ElementDocument* document = context->LoadDocument(assetDir + "/ui/poc.rml");
        allPassed = check(document != nullptr, "Document geladen (poc.rml)") && allPassed;
        if (document == nullptr)
        {
            return false;
        }
        document->Show();
        context->Update();

        // 4. Elementen terugvinden en layout inspecteren.
        Rml::Element* label = document->GetElementById("label");
        Rml::Element* button = document->GetElementById("startButton");
        allPassed = check(label != nullptr && button != nullptr, "Elementen gevonden via id") && allPassed;
        if (label == nullptr || button == nullptr)
        {
            return false;
        }

        allPassed = check(label->GetInnerRML() == "Hello RmlUi", "Label bevat de verwachte tekst") && allPassed;
        allPassed = check(label->GetOffsetWidth() > 0.0f, "Label heeft een breedte (lettertype werkt)") && allPassed;
        allPassed = check(button->GetAbsoluteTop() > label->GetAbsoluteTop(),
                          "Knop staat onder het label (layout klopt)") &&
                    allPassed;

        std::cout << "       label : x=" << label->GetAbsoluteLeft() << " y=" << label->GetAbsoluteTop()
                  << " b=" << label->GetOffsetWidth() << " h=" << label->GetOffsetHeight() << '\n';
        std::cout << "       knop  : x=" << button->GetAbsoluteLeft() << " y=" << button->GetAbsoluteTop()
                  << " b=" << button->GetOffsetWidth() << " h=" << button->GetOffsetHeight() << '\n';

        // 5. Klik simuleren zonder echte muis: beweeg naar het midden van de
        //    knop en druk/laat los.
        poc::ClickListener listener(label);
        button->AddEventListener("click", &listener);

        const int clickX = static_cast<int>(button->GetAbsoluteLeft() + button->GetOffsetWidth() / 2.0f);
        const int clickY = static_cast<int>(button->GetAbsoluteTop() + button->GetOffsetHeight() / 2.0f);
        context->ProcessMouseMove(clickX, clickY, 0);
        context->ProcessMouseButtonDown(0, 0);
        context->ProcessMouseButtonUp(0, 0);
        context->Update();

        allPassed = check(listener.getClickCount() == 1, "Klik-event bereikt de listener") && allPassed;
        allPassed = check(label->GetInnerRML() == "Clicked 1 time(s)", "UI is dynamisch bijgewerkt na de klik") &&
                    allPassed;

        // De listener moet losgekoppeld zijn voordat hij buiten bereik valt,
        // anders verwijst RmlUi bij het opruimen naar een verwijderd object.
        button->RemoveEventListener("click", &listener);

        // 6. Renderen: de null render interface ontvangt de tekencommando's.
        renderInterface.resetFrameStatistics();
        context->Render();

        const poc::RenderStatistics& statistics = renderInterface.getStatistics();
        allPassed = check(statistics.renderCalls > 0, "RmlUi genereert tekencommando's bij Render()") && allPassed;

        std::cout << "\nWat RmlUi zou tekenen in 1 frame:\n"
                  << "       RenderGeometry-aanroepen : " << statistics.renderCalls << '\n'
                  << "       driehoeken               : " << statistics.trianglesRendered << '\n'
                  << "       scissor-wijzigingen      : " << statistics.scissorRegionChanges << '\n'
                  << "       textures gegenereerd     : " << statistics.texturesGenerated << " (totaal)\n"
                  << "       geometrie gecompileerd   : " << statistics.geometryCompiled << " (totaal)\n";

        return allPassed;
    }
} // namespace

auto main() -> int
{
    poc::NullRenderInterface renderInterface;
    poc::LoggingSystemInterface systemInterface;

    // De interfaces moeten ingesteld zijn vóór Initialise() en moeten
    // blijven bestaan tot ná Shutdown().
    Rml::SetSystemInterface(&systemInterface);
    Rml::SetRenderInterface(&renderInterface);

    if (!Rml::Initialise())
    {
        std::cerr << "Rml::Initialise() mislukt\n";
        return EXIT_FAILURE;
    }

    const bool success = runChecks(renderInterface);

    Rml::Shutdown();

    std::cout << '\n' << (success ? "Alle checks geslaagd." : "Er zijn checks mislukt.") << '\n';
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
