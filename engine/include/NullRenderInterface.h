#pragma once

#include <RmlUi/Core/RenderInterface.h>

#include <cstddef>
#include <unordered_map>

namespace poc
{
    // Telt wat RmlUi zou tekenen. Dit is meteen de informatie die nodig is om
    // later te beoordelen wat een IRenderer moet kunnen om RmlUi te voeden.
    struct RenderStatistics
    {
        std::size_t geometryCompiled = 0;
        std::size_t renderCalls = 0;
        std::size_t trianglesRendered = 0;
        std::size_t texturesGenerated = 0;
        std::size_t scissorRegionChanges = 0;
    };

    // Render interface zonder uitvoer: RmlUi doet de parsing, styling en
    // layout, maar er wordt niets getekend. De methodenamen komen van RmlUi
    // zelf (overrides), vandaar PascalCase.
    class NullRenderInterface final : public Rml::RenderInterface
    {
    public:
        NullRenderInterface() = default;

        Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                    Rml::Span<const int> indices) override;
        void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation,
                            Rml::TextureHandle texture) override;
        void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

        Rml::TextureHandle LoadTexture(Rml::Vector2i& textureDimensions, const Rml::String& source) override;
        Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source,
                                           Rml::Vector2i sourceDimensions) override;
        void ReleaseTexture(Rml::TextureHandle texture) override;

        void EnableScissorRegion(bool enable) override;
        void SetScissorRegion(Rml::Rectanglei region) override;

        [[nodiscard]] auto getStatistics() const -> const RenderStatistics&;
        auto resetFrameStatistics() -> void;

    private:
        RenderStatistics statistics;

        // Handle -> aantal indices, zodat RenderGeometry kan tellen
        // hoeveel driehoeken er getekend zouden worden.
        std::unordered_map<Rml::CompiledGeometryHandle, std::size_t> geometryIndexCounts;

        Rml::CompiledGeometryHandle nextGeometryHandle = 1;
        Rml::TextureHandle nextTextureHandle = 1;
    };
} // namespace poc
