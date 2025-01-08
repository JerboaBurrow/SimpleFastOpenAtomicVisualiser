#ifndef VKPARTICLES
#define VKPARTICLES

#include <jGL/particles.h>

namespace jGL::Vulkan
{
    class vkParticles : public Particles
    {
    public:

        vkParticles(size_t sizeHint)
        : Particles(sizeHint)
        {}

        void update(Particles::UpdateInfo info) {TODO("jGL::Vulkan::vkParticles::update");}

        void draw(glm::mat4 proj) {TODO("jGL::Vulkan::vkParticles::draw");}

        void setTexture(std::shared_ptr<Texture>) {TODO("jGL::Vulkan::vkParticles::setTexture");}
    };
}

#endif /* VKPARTICLES */
