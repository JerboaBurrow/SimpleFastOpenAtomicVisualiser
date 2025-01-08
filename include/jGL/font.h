#ifndef FONT_H
#define FONT_H

#include <jGL/glyph.h>
#include <jGL/texture.h>

#include <map>
#include <algorithm>


namespace jGL
{
    class Font 
    {

    public:

        Font(uint8_t w);

        virtual ~Font() {}

        const glm::ivec2 getBitmapSize() const { return glm::ivec2(fontBitmap->size().x, fontBitmap->size().y); }

        std::array<glm::vec4, 6> getGlyphVertices(float x, float y, float scale, unsigned char c);
        glm::vec4 getGlyphOffset(unsigned char c) { return glyphOffset[c]; }
        glm::ivec2 getGlyphSize(unsigned char c) { return glyphs[c].getSize(); }

        float spacing(float scale) { return scale * width; }

        void bind(unsigned int b) { fontBitmap->bind(b); }

    protected:

        uint8_t width;

        std::map<unsigned char, Glyph> glyphs;
        std::map<unsigned char, glm::vec4> glyphOffset;

        std::shared_ptr<Texture> fontBitmap;
        std::vector<std::byte> bitmapPixels;
        uint16_t bw, bh;

    };
}

#endif /* FONT_H */
