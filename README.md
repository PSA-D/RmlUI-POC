# RmlUi POC (headless)

Proof of concept voor RmlUi 6.3 zonder venster, SDL of GPU. Het POC laadt een
UI-document, berekent de layout, handelt events af en genereert rendercommando's,
allemaal met een render interface die (nog) niets tekent.

## Onderzoeksvraag

Kan RmlUi in onze CMake/vcpkg-setup gebouwd worden, een document met een paar
UI-elementen laden, de layout berekenen, events afhandelen en rendercommando's
genereren, onafhankelijk van een renderer, venster of SDL?

Dit POC onderzoekt uitsluitend de integratie van de RmlUi-core. De koppeling
tussen RmlUi's renderdata en de engine-renderer wordt hier niet getest.

## Conclusie

RmlUi-core werkt volledig onafhankelijk van een renderer. Parsing, styling,
layout, events en documentbeheer vereisen alleen de interfaces die de
applicatie zelf aanlevert. De UI-logica is daarmee los te koppelen van de
rendering.

## Bevindingen

**Onafhankelijkheid van de renderer**

- RmlUi heeft geen venster, SDL of GPU nodig voor parsing, styling, layout,
  event-afhandeling en documentbeheer. Dit werkt volledig met een
  `NullRenderInterface`.
- Een document wordt vanuit een `.rml`-bestand geladen en opgebouwd tot een
  werkende UI-hiërarchie.

**Lettertypen en layout**

- Lettertypen worden geladen via FreeType. Zonder geladen lettertype krijgen
  tekstelementen geen bruikbare layout-afmetingen.
- Layoutberekening is deterministisch en controleerbaar via code. Posities en
  afmetingen van elementen zijn direct uit de RmlUi-context uit te lezen.
- Elementen zijn op te zoeken via hun id.

**Events**

- Muisevents zijn volledig programmatisch te simuleren met
  `ProcessMouseMove`, `ProcessMouseButtonDown` en `ProcessMouseButtonUp`.
- Event listeners ontvangen klik-events en passen de UI dynamisch aan, bijvoorbeeld
  met `SetInnerRML()`.

**Rendercommando's**

- RmlUi genereert rendercommando's zonder actieve renderer. Het POC voert
  `Render()` uit en laat de render interface registreren welke aanroepen
  zouden plaatsvinden.
- RmlUi 6.3 gebruikt naast geometrie ook texture-functionaliteit:
  `LoadTexture`, `GenerateTexture` en `ReleaseTexture`. Zelfs zonder
  afbeeldingen in het document wordt `GenerateTexture()` aangeroepen voor de
  glyph-atlas van het geladen lettertype.
- Het testdocument genereert:
  - 5 `RenderGeometry()`-aanroepen
  - 52 driehoeken
  - 1 gegenereerde texture (glyph-atlas)

## Gevalideerde functionaliteit

Het POC controleert automatisch:

1. Laden van een TrueType-lettertype.
2. Aanmaken van een RmlUi-context.
3. Laden van een `.rml`-document.
4. Opzoeken van elementen via id.
5. Correcte tekstinhoud van UI-elementen.
6. Correcte layout-afmetingen na het laden van het lettertype.
7. Correcte positionering van de knop ten opzichte van het label.
8. Correcte aflevering van klik-events aan een listener.
9. Dynamische aanpassing van de UI na een klik.
10. Generatie van rendergegevens tijdens `Render()`.

Het programma sluit alleen met exitcode 0 af als alle controles slagen.

## Wat dit niet bewijst

- Visuele correctheid van rendering.
- Integratie met SDL.
- Integratie met OpenGL, Vulkan of DirectX.
- Laden en renderen van afbeeldingen.
- Font-renderkwaliteit.
- Integratie met de engine-renderer.

RmlUi levert getextureerde driehoeksgeometrie inclusief clipping-regio's. Dit
POC toont aan dat deze data beschikbaar komt, maar test niet hoe de data op de
engine-renderlaag wordt aangesloten.

## Geverifieerde configuratie

Getest met:

| Onderdeel | Versie |
|---|---|
| Besturingssysteem | Windows |
| Compiler | MSVC (Visual Studio 2022 toolchain) |
| IDE | CLion |
| CMake | 3.25 of nieuwer |
| Dependency-beheer | vcpkg (manifest mode) |
| RmlUi | 6.3 |
| FreeType | via de vcpkg-port van RmlUi |

## Installatie

### 1. vcpkg

Zorg dat vcpkg beschikbaar is, bijvoorbeeld in `C:\dev\vcpkg`, en stel de
omgevingsvariabele in:

```text
VCPKG_ROOT=C:\dev\vcpkg
```

Start CLion of de terminal opnieuw na het instellen van de variabele.

### 2. Dependency

`vcpkg.json` in de projectroot bevat:

```json
{
    "name": "rmlui-poc",
    "version": "0.1.0",
    "dependencies": [
        "rmlui"
    ]
}
```

vcpkg installeert RmlUi automatisch tijdens de eerste CMake-configure. De
eerste keer duurt dit enkele minuten. Handmatig installeren kan met:

```powershell
C:\dev\vcpkg\vcpkg.exe install
```

Controleer de installatie in de map `vcpkg_installed/x64-windows/` in de
buildmap: die bevat de RmlUi-bibliotheken.

### 3. CLion configureren

Gebruik een CMake-profiel dat de vcpkg-toolchain gebruikt:

```text
Settings → Build, Execution, Deployment → CMake → CMake options:
-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Na het toevoegen of wijzigen van dependencies:

```text
Tools → CMake → Reset Cache and Reload Project
```

## Bouwen en draaien

**CLion:** kies de run-configuratie `Game` en klik op Run.

**Command line (PowerShell):**

```powershell
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build
.\build\game\Game.exe
```

Het assetpad is als absoluut pad in de build ingebakken, dus de werkmap van
het proces is niet relevant.

### Verwachte uitvoer

```text
[PASS] Lettertype geladen
[PASS] Context aangemaakt
[PASS] Document geladen (poc.rml)
[PASS] Elementen gevonden via id
[PASS] Label bevat de verwachte tekst
[PASS] Label heeft een breedte (lettertype werkt)
[PASS] Knop staat onder het label (layout klopt)
       label : x=35 y=20 b=106 h=21.6
       knop  : x=20 y=41.6 b=160 h=41.6
[PASS] Klik-event bereikt de listener
[PASS] UI is dynamisch bijgewerkt na de klik
[PASS] RmlUi genereert tekencommando's bij Render()

Wat RmlUi zou tekenen in 1 frame:
       RenderGeometry-aanroepen : 5
       driehoeken               : 52
       scissor-wijzigingen      : 0
       textures gegenereerd     : 1 (totaal)
       geometrie gecompileerd   : 5 (totaal)

Alle checks geslaagd.
```

Exitcode `0` betekent dat alle controles geslaagd zijn. De RmlUi-logmelding
over het geladen lettertype verschijnt op stderr.

## Projectstructuur

```text
RmlUI-POC/
├── assets/
│   ├── fonts/DejaVuSans.ttf
│   └── ui/poc.rml
├── engine/
│   ├── include/
│   │   ├── NullRenderInterface.h
│   │   └── LoggingSystemInterface.h
│   └── src/
│       └── ui/
│           ├── NullRenderInterface.cpp
│           └── LoggingSystemInterface.cpp
├── game/
│   └── src/
│       ├── main.cpp
│       ├── ClickListener.h
│       └── ClickListener.cpp
└── vcpkg.json
```

| Bestand | Rol |
|---|---|
| `main.cpp` | Initialiseert RmlUi, voert de controles uit en ruimt op. |
| `NullRenderInterface` | Render interface die niets tekent en telt wat RmlUi zou tekenen. |
| `LoggingSystemInterface` | Levert RmlUi de verstreken tijd en toont RmlUi-logmeldingen in de console. |
| `ClickListener` | Event listener die klikken telt en het label bijwerkt. |
| `assets/ui/poc.rml` | Testdocument met een paneel, label en knop, inclusief RCSS-stijl. |
| `assets/fonts/DejaVuSans.ttf` | Lettertype. DejaVu heeft een vrije licentie die herdistributie toestaat. |

## CMake-integratie

RmlUi wordt gevonden via:

```cmake
find_package(RmlUi CONFIG REQUIRED)
```

en gekoppeld aan de engine:

```cmake
target_include_directories(engine PUBLIC include)

target_link_libraries(engine PUBLIC
    project_sanitizers
    project_warnings
    RmlUi::Core
)
```

De vcpkg-port van RmlUi 6.3 exporteert de targets `RmlUi::Core`,
`RmlUi::Debugger` en `RmlUi::RmlUi`.

De game-executable linkt tegen de engine en krijgt het assetpad mee:

```cmake
target_link_libraries(Game PRIVATE engine)

target_compile_definitions(Game PRIVATE
    POC_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
)
```

## Werking van het POC

RmlUi tekent zelf niets. De applicatie levert twee interfaces aan:

| Interface | Verplicht | Taak |
|---|---|---|
| `Rml::RenderInterface` | Ja | Ontvangt geometrie, textures en scissor-regio's om te tekenen. |
| `Rml::SystemInterface` | Nee | Levert tijd en ontvangt logmeldingen. |

`NullRenderInterface` implementeert alle verplichte methoden van RmlUi 6.3:

- Geometrie: `CompileGeometry`, `RenderGeometry`, `ReleaseGeometry`
- Textures: `LoadTexture`, `GenerateTexture`, `ReleaseTexture`
- Clipping: `EnableScissorRegion`, `SetScissorRegion`

Volgorde van het POC in `main()`:

1. `Rml::SetSystemInterface()` en `Rml::SetRenderInterface()`
2. `Rml::Initialise()`
3. `Rml::LoadFontFace()`
4. `Rml::CreateContext()` en `context->LoadDocument()`
5. Controles uitvoeren (layout, klik, render)
6. `Rml::Shutdown()`

Regels bij het gebruik van RmlUi:

- De interfaces worden ingesteld vóór `Rml::Initialise()` en blijven bestaan
  tot ná `Rml::Shutdown()`.
- RmlUi bezit de context, het document en de elementen. Pointers naar deze
  objecten zijn niet-bezittend.
- Een event listener wordt met `RemoveEventListener()` losgekoppeld voordat het
  listener-object wordt vernietigd. RmlUi benadert de listener anders bij het
  opruimen van het element.
- Een lettertype wordt geladen vóór het document dat het gebruikt.

## Probleemoplossing

| Melding of symptoom | Oorzaak en oplossing |
|---|---|
| `Could not find a package configuration file provided by "RmlUi"` | De vcpkg-toolchain is niet meegegeven. Stel `CMAKE_TOOLCHAIN_FILE` in en voer Reset Cache and Reload Project uit. |
| `VCPKG_ROOT` is leeg | De omgevingsvariabele is niet gezet, of CLion of de terminal is niet opnieuw gestart. |
| `[RmlUi ERROR]` over het lettertype en label zonder breedte | Het lettertype is niet gevonden. Controleer dat `assets/fonts/DejaVuSans.ttf` bestaat. |
| Document wordt niet geladen | Controleer dat `assets/ui/poc.rml` bestaat. Het pad komt uit `POC_ASSET_DIR`. |
| Configure duurt lang | De eerste keer bouwt vcpkg RmlUi en FreeType. |


Het assetpad wordt door CMake als absoluut pad ingebakken
(`POC_ASSET_DIR`), dus het programma werkt vanuit elke werkmap. Het
afsluiten met exitcode 0 betekent dat alle checks geslaagd zijn.

## Structuur

| Bestand | Rol |
|---|---|
| `main.cpp` | Initialiseert RmlUi, voert de checks uit, ruimt op. |
| `NullRenderInterface.*` | Render interface die niets tekent maar telt wat RmlUi zou tekenen. |
| `LoggingSystemInterface.*` | Levert tijd en toont RmlUi-logmeldingen in de console. |
| `ClickListener.*` | Event listener die klikken telt en het label bijwerkt. |
| `assets/ui/poc.rml` | Document met een paneel, label en knop (incl. RCSS-stijl). |
| `assets/fonts/DejaVuSans.ttf` | Lettertype (DejaVu, vrije licentie die herdistributie toestaat). |
