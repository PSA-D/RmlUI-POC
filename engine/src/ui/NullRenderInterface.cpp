#include "NullRenderInterface.h"

namespace poc
{
    Rml::CompiledGeometryHandle NullRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> /*vertices*/,
                                                                     Rml::Span<const int> indices)
    {
        const Rml::CompiledGeometryHandle handle = nextGeometryHandle;
        ++nextGeometryHandle;

        geometryIndexCounts[handle] = indices.size();
        ++statistics.geometryCompiled;
        return handle;
    }

    void NullRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f /*translation*/,
                                             Rml::TextureHandle /*texture*/)
    {
        const auto found = geometryIndexCounts.find(geometry);
        if (found == geometryIndexCounts.end())
        {
            return;
        }

        ++statistics.renderCalls;
        statistics.trianglesRendered += found->second / 3;
    }

    void NullRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
    {
        geometryIndexCounts.erase(geometry);
    }

    Rml::TextureHandle NullRenderInterface::LoadTexture(Rml::Vector2i& /*textureDimensions*/,
                                                        const Rml::String& /*source*/)
    {
        // Headless POC zonder afbeeldingen: 0 betekent "laden mislukt".
        return 0;
    }

    Rml::TextureHandle NullRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> /*source*/,
                                                            Rml::Vector2i /*sourceDimensions*/)
    {
        // RmlUi gebruikt dit o.a. voor de glyph-atlas van lettertypes.
        const Rml::TextureHandle handle = nextTextureHandle;
        ++nextTextureHandle;

        ++statistics.texturesGenerated;
        return handle;
    }

    void NullRenderInterface::ReleaseTexture(Rml::TextureHandle /*texture*/)
    {
    }

    void NullRenderInterface::EnableScissorRegion(bool /*enable*/)
    {
        ++statistics.scissorRegionChanges;
    }

    void NullRenderInterface::SetScissorRegion(Rml::Rectanglei /*region*/)
    {
        ++statistics.scissorRegionChanges;
    }

    auto NullRenderInterface::getStatistics() const -> const RenderStatistics&
    {
        return statistics;
    }

    auto NullRenderInterface::resetFrameStatistics() -> void
    {
        statistics.renderCalls = 0;
        statistics.trianglesRendered = 0;
        statistics.scissorRegionChanges = 0;
    }
} // namespace poc
