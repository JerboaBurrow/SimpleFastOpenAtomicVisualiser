#ifndef GLFONT_H
#define GLFONT_H

#include <jGL/font.h>
#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Texture/glTexture.h>

#include <map>

namespace jGL::GL
{
    class glFont : public Font
    {
    
    public:

        glFont(uint8_t w)
        : Font(w)
        {
            upload(bw, bh);
        }

        ~glFont() {}

    private:

        void upload(uint16_t w, uint16_t h);

    };
}

#endif /* GLFONT_H */
