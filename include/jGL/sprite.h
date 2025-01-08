#ifndef SPRITE_H
#define SPRITE_H

#include <jGL/texture.h>
#include <jGL/primitive.h>
#include <jGL/orthoCam.h>

#include <memory>
#include <algorithm>

/**
 * @brief A drawable graphic.
 *
 * Observes a Transform (position, orientation, scale), and a Texture.
 *
 * Rendered using the TextureRegion (pixel units) region of the Texture, at the alpha value.
 *
 */

namespace jGL
{
    class Sprite
    {

    public:

        Sprite() = default;

        /**
         * @brief Construct a Sprite
         *
         * @param tra x, y, theta, scale Transform
         * @param to Region of texture to draw
         * @param tex Texture to draw
         * @param alpha Transparency modifier (alpha * texture alpha)
         */
        Sprite
        (
            Transform * tra,
            TextureRegion * to,
            Texture * tex,
            glm::vec4 * colour
        )
        : transform(tra),
          texOffset(to),
          texture(tex),
          colour(colour)
        {}

        Transform * transform;
        TextureRegion * texOffset;
        Texture * texture;
        glm::vec4 * colour;

        /**
         * @brief Get the WorldBoundingBox of the Sprite.
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
         * @brief Get the ScreenBoundingBox of the Sprite.
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

        NormalisedTextureRegion getNormalisedTextureRegion() const
        {
            return toNormalised(*texOffset);
        }

    protected:

        NormalisedTextureRegion toNormalised(TextureRegion to) const
        {
            glm::vec3 whc = texture->size();
            if (to.lx == 0){ to.lx = whc.x; }
            if (to.ly == 0){ to.ly = whc.y; }
            return NormalisedTextureRegion(
                std::clamp(float(to.tx) / float(whc.x), 0.0f, 1.0f),
                std::clamp(float(to.ty) / float(whc.y), 0.0f, 1.0f),
                std::clamp(float(to.lx) / float(whc.x), 0.0f, 1.0f),
                std::clamp(float(to.ly) / float(whc.y), 0.0f, 1.0f)
            );
        }

        TextureRegion fromNormalised(NormalisedTextureRegion to) const
        {
            glm::vec3 whc = texture->size();
            return TextureRegion(
                uint16_t(to.tx * whc.x),
                uint16_t(to.ty * whc.y),
                uint16_t(to.lx * whc.x),
                uint16_t(to.ly * whc.y)
            );
        }
    };
}

#endif /* SPRITE_H */
