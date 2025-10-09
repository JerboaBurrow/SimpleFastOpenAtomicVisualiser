#ifndef OPENGLINSTANCE_H
#define OPENGLINSTANCE_H

#include <jGL/jGL.h>
#include <jGL/OpenGL/Text/glTextRenderer.h>
#include <jGL/OpenGL/glParticles.h>
#include <jGL/OpenGL/glSpriteRenderer.h>
#include <jGL/OpenGL/glShapeRenderer.h>
#include <jGL/OpenGL/Texture/glTexture.h>
#include <jGL/OpenGL/glDrawFramebuffer.h>

namespace jGL::GL
{

    class glParticles;

    class OpenGLInstance : public jGLInstance
    {

        public:

            OpenGLInstance(glm::ivec2 res, uint8_t msaa = 0)
            : jGLInstance(res),
              framebuffer(glDrawFramebuffer()),
              textRenderer()
            {
                framebuffer.setResolution
                (
                    res
                );
                framebuffer.setMSAA(msaa);
                setTextProjection(glm::ortho(0.0,double(resolution.x),0.0,double(resolution.y)));
            }

            ~OpenGLInstance()
            {
                if (glIsBuffer(rbo)) { glDeleteRenderbuffers(1, &rbo); }
                framebuffer.destroy();
            }

            void beginFrame(){framebuffer.bind();}
            void endFrame(){framebuffer.blit();}

            void finish(){closing = true;}

            void text
            (
                std::string characters,
                glm::vec2 position,
                float scale,
                glm::vec4 colour,
                glm::bvec2 centre = glm::bvec2(false,false)
            );

            void clear()
            {
                textRenderer.clear();
                glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                framebuffer.setClear(clearColour);
            }

            void setMSAA(uint8_t samples)
            {
                if (samples == 0) { msaaSamples = 0; return; }
                GLint maxSamples;
                glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

                if (samples > maxSamples){samples = maxSamples;}

                msaaSamples = samples;

                framebuffer.setMSAA(samples);

                if (glIsBuffer(rbo)) { glDeleteRenderbuffers(1, &rbo); }
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            }

            void setResolution(glm::ivec2 wh)
            {
                resolution = wh;
                framebuffer.setResolution(resolution);
                setMSAA(msaaSamples);
                setTextProjection(glm::ortho(0.0,double(resolution.x),0.0,double(resolution.y)));
                glViewport(0.0f, 0.0f, resolution.x, resolution.y);
            }

            void setClear(glm::vec4 colour) { clearColour = colour; }
            void setProjection(glm::mat4 proj) {/*TODO*/}
            void setTextProjection(glm::mat4 proj) { textRenderer.setProjection(proj); }
            void setViewport(glm::vec4 view) { glViewport(view.x, view.y, view.z, view.w); }

            std::shared_ptr<Particles> createParticles(size_t sizeHint)
            {
                return std::static_pointer_cast<Particles>(std::make_shared<glParticles>(sizeHint));
            }

            std::shared_ptr<Texture> createTexture(std::filesystem::path imageFile, Texture::Type type)
            {
                switch (type)
                {
                    case Texture::Type::RGB:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(imageFile));
                    case Texture::Type::RGBA:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGBA>(imageFile));
                    default:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(imageFile));
                }
            }

            std::shared_ptr<Texture> createTexture(std::vector<std::byte> data, Texture::Type type)
            {
                switch (type)
                {
                    case Texture::Type::RGB:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(data));
                    case Texture::Type::RGBA:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGBA>(data));
                    default:
                        return std::static_pointer_cast<Texture>(std::make_shared<glTexture2DRGB>(data));
                }
            }

            std::shared_ptr<SpriteRenderer> createSpriteRenderer(size_t sizeHint)
            {
                return std::static_pointer_cast<SpriteRenderer>
                (
                    std::make_shared<glSpriteRenderer>
                    (
                        sizeHint
                    )
                );
            }

            std::shared_ptr<ShapeRenderer> createShapeRenderer(size_t sizeHint)
            {
                return std::static_pointer_cast<ShapeRenderer>
                (
                    std::make_shared<glShapeRenderer>
                    (
                        sizeHint
                    )
                );
            }

        private:

            glDrawFramebuffer framebuffer;

            glFont defaultFont = glFont(48);

            TextRenderer textRenderer;

            glm::vec4 clearColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            GLuint rbo;
    };
}

#endif /* OPENGLINSTANCE_H */
