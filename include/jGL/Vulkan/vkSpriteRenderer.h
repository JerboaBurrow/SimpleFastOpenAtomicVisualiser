#ifndef VKSPRITERENDERER
#define VKSPRITERENDERER

#include <jGL/Vulkan/Device/device.h>
#include <jGL/spriteRenderer.h>

namespace jGL::Vulkan
{
    class vkSpriteRenderer : public SpriteRenderer
    {
    public:

        vkSpriteRenderer(size_t sizeHint)
        : SpriteRenderer(sizeHint)
        {}
    private:
        void draw
        (
            std::shared_ptr<Shader> shader,
            std::vector<std::pair<Info, Sprite>> & sprites
        ){}
    };
}

#endif /* VKSPRITERENDERER */
