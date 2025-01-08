#ifndef PARTICLES
#define PARTICLES

#include <jGL/primitive.h>
#include <jGL/texture.h>

#include <memory>
#include <vector>
#include <cstdint>

namespace jGL
{
    class Particles
    {

    public:

        struct UpdateInfo
        {
            bool position, colour, texCoord;
        };

        Particles(size_t sizeHint)
        : points(std::vector<TexturedParticle>(sizeHint))
        {}

        Particles(std::vector<TexturedParticle> p)
        : points(p)
        {}

        virtual ~Particles() = default;

        virtual void update(UpdateInfo info) = 0;

        virtual void draw(glm::mat4 proj) = 0;

        virtual void setTexture(std::shared_ptr<Texture>) = 0;

        std::vector<TexturedParticle> & getParticles(){ return points; }

        void setBaseLineScale(float s) { baseLineScalePixels = s; }

    protected:

        float baseLineScalePixels = 1.0f;

        std::vector<TexturedParticle> points;
        std::shared_ptr<Texture> texture;

        uint32_t uploaded = 0;
        uint32_t drawing = 0;

    };
}

#endif /* PARTICLES */
