#ifndef GLDRAWFRAMEBUFFER
#define GLDRAWFRAMEBUFFER

#include <cstdint>

#include <glm/glm.hpp>

#include <jGL/OpenGL/gl.h>

namespace jGL::GL
{
    class glDrawFramebuffer
    {

    public:

        glDrawFramebuffer()
        {
            if(!glIsFramebuffer(buffer))
            {
                glGenFramebuffers(1, &buffer);
            }

            if(!glIsTexture(texture))
            {
                glGenTextures(1, &texture);
            }
        }

        void destroy()
        {
            if (glIsFramebuffer(buffer))
            {
                glDeleteFramebuffers(1, &buffer);
            }

            if (glIsTexture(texture))
            {
                glDeleteTextures(1, &texture);
            }
        }

        void bind()
        {
            if (msaaSamples == 0)
            {
                return;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            glBufferStatus("bind framebuffer");
        }

        void blit()
        {
            if (msaaSamples == 0)
            {
                return;
            }
            glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, resX, resY, 0, 0, resX, resY, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glError("blit framebuffer");
        }

        void setClear(glm::vec4 c)
        {
            clearColour = c;
            glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
        }

        void clear() { glClear(GL_COLOR_BUFFER_BIT); }

        void setResolution(glm::vec2 res)
        {
            resX = res.x;
            resY = res.y;
        }

        void setMSAA(uint8_t samples)
        {
            msaaSamples = samples;

            if (msaaSamples == 0)
            {
                return;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, buffer);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
                // I fucking love cross platform development...
                #ifdef ANDROID
                    glTexStorage2DMultisample
                    (
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        msaaSamples, 
                        GL_RGB8, 
                        resX, 
                        resY, 
                        GL_TRUE
                    );
                #else
                    glTexImage2DMultisample
                    (
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        msaaSamples, 
                        GL_RGB, 
                        resX, 
                        resY, 
                        GL_TRUE
                    );
                #endif
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); 
            
            glFramebufferTexture2D
            (
                GL_FRAMEBUFFER, 
                GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_2D_MULTISAMPLE, 
                texture, 
                0
            ); 

            setClear(clearColour);
        }

        static const bool isMSAA() {return msaaSamples > 0;}
    
    private:

        static bool initialised;

        static uint8_t msaaSamples;

        static GLuint buffer, texture;

        static uint16_t resX, resY;

        static glm::vec4 clearColour;

    };
}

#endif /* GLDrawFRAMEBUFFER */
