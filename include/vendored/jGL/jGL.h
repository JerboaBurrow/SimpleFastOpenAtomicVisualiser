#ifndef JGL
#define JGL

#include <memory>
#include <filesystem>

#include <jGL/Display/desktopDisplay.h>

#include <jGL/texture.h>
#include <jGL/particles.h>
#include <jGL/spriteRenderer.h>
#include <jGL/shapeRenderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL
{

    class jGLInstance
    {

    public:

        jGLInstance(glm::ivec2 res)
        : resolution(res), closing(false)
        {}

        virtual ~jGLInstance() = default;

        virtual void text
        (
            std::string characters, 
            glm::vec2 position,
            float scale,
            glm::vec4 colour,
            glm::bvec2 centre = glm::bvec2(false,false)
        ) = 0;

        virtual void finish(){closing = true;}
        virtual void beginFrame(){}
        virtual void endFrame(){}

        virtual void clear() = 0;

        virtual void setClear(glm::vec4 colour) = 0;
        virtual void setProjection(glm::mat4 proj) = 0;
        virtual void setTextProjection(glm::mat4 proj) = 0;
        virtual void setViewport(glm::vec4 view) = 0;

        virtual void setMSAA(uint8_t samples) {msaaSamples = samples;}

        virtual std::shared_ptr<Particles> createParticles(size_t sizeHint) = 0;
        virtual std::shared_ptr<Texture> createTexture(std::filesystem::path imageFile, Texture::Type type) = 0;
        virtual std::shared_ptr<Texture> createTexture(std::vector<std::byte> data, Texture::Type type) = 0;
        virtual std::shared_ptr<SpriteRenderer> createSpriteRenderer(size_t sizeHint) = 0;
        virtual std::shared_ptr<ShapeRenderer> createShapeRenderer(size_t sizeHint) = 0;

        const bool isClosing() const { return closing; } 

    protected:

        glm::ivec2 resolution;
        uint8_t msaaSamples = 0;
        bool closing = false;

    };
}

#endif /* JGL */
