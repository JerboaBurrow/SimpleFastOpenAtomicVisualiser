#ifndef VKSHAPERENDERER_H
#define VKSHAPERENDERER_H

#include <jGL/Vulkan/Device/device.h>
#include <jGL/shapeRenderer.h>

namespace jGL::Vulkan
{
    class vkShapeRenderer : public ShapeRenderer
    {
    public:

        vkShapeRenderer(size_t sizeHint)
        : ShapeRenderer(sizeHint)
        {}
    protected:
        void draw
        (
            std::shared_ptr<Shader> shader,
            std::vector<std::pair<Info, Shape>> & shapes,
            UpdateInfo info = UpdateInfo()
        ){}
    };
}

#endif /* VKSHAPERENDERER_H */
