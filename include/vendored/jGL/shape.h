#ifndef SHAPE_H
#define SHAPE_H

#include <jGL/primitive.h>
#include <jGL/orthoCam.h>

namespace jGL
{
    /**
     * @brief A drawable shape
     * @remark Detailed geometry is assumed defined in the shader. Circle and Rectangle are provided in ShapeRenderer.
     */
    class Shape
    {

    public:

        Shape()
        : transform(nullptr), colour(nullptr)
        {}

        Shape(const Transform * tra, const glm::vec4 * c)
        : transform(tra), colour(c)
        {}

        const Transform * transform;
        const glm::vec4 * colour;

        /**
         * @brief Get the WorldBoundingBox of the Shape.
         *
         * @return WorldBoundingBox
         */
        WorldBoundingBox getWorldBoundingBox() const
        {
            WorldBoundingBox wbb =
            {
                {
                    glm::vec2(-0.5, -0.5),
                    glm::vec2(-0.5, 0.5),
                    glm::vec2(0.5, 0.5),
                    glm::vec2(0.5, -0.5)
                }
            };

            float ct = std::cos(transform->theta); float st = std::sin(transform->theta);
            glm::mat2 rot(ct, -st, st, ct);
            glm::vec2 pos(transform->x, transform->y);
            glm::vec2 scale(transform->scaleX, transform->scaleY);


            for (uint8_t i = 0; i < wbb.vertices.size(); i++)
            {
                wbb.vertices[i] = rot*(wbb.vertices[i]*scale)+pos;
            }

            return wbb;
        }

        /**
         * @brief Get the ScreenBoundingBox of the Shape.
         *
         * @param camera for projection to the screen.
         * @return ScreenBoundingBox
         */
        ScreenBoundingBox getScreenBoundingBox(const OrthoCam & camera)
        {
            WorldBoundingBox wbb = getWorldBoundingBox();
            ScreenBoundingBox sbb;
            glm::vec2 pos;
            for (uint8_t i = 0; i < wbb.vertices.size(); i++)
            {
                pos = camera.worldToScreen(wbb.vertices[i].x, wbb.vertices[i].y);
                sbb.vertices[i].x = uint16_t(pos.x);
                sbb.vertices[i].y = uint16_t(pos.y);
            }
            return sbb;
        }

    };
}

#endif /* SHAPE_H */
