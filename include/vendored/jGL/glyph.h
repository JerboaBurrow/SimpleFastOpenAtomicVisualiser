#ifndef GLYPH_H
#define GLYPH_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <jGL/defaultFont.h>

#include <glm/glm.hpp>

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <array>

namespace jGL
{
    class Glyph
    {
    
    public:

        Glyph() = default;

        Glyph
        (
            const FT_Face & face, 
            unsigned char ch
        );

        std::vector<unsigned char> & getPixels() {return bitmap;}

        glm::ivec2 getSize() const
        {
            return size;
        }

        std::array<glm::vec2, 6> vertices(float x, float y, float scale)
        {
            float xpos = x + bearing.x * scale;
            float ypos = y + (size.y - bearing.y) * scale;

            float w = size.x * scale;
            float h = size.y * scale;

            std::array<glm::vec2, 6> v;
            v[0] = glm::vec2(xpos,   ypos-h);
            v[1] = glm::vec2(xpos,   ypos);
            v[2] = glm::vec2(xpos+w, ypos);
            v[3] = glm::vec2(xpos,   ypos-h);
            v[4] = glm::vec2(xpos+w, ypos);
            v[5] = glm::vec2(xpos+w, ypos-h);

            return v;
        }


    private:

        std::vector<unsigned char> bitmap;
        unsigned char character;
        glm::ivec2 size, bearing;
        uint64_t offset;

    };
}

#endif /* GLYPH_H */
