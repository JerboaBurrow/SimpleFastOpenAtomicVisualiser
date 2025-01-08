#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <jGL/sprite.h>
#include <jGL/shader.h>
#include <jGL/priorityStore.h>

#include <unordered_map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

namespace jGL
{
    /**
     * @brief User name for a Sprite.
     * @typedef SpriteId
     * */
    typedef std::string SpriteId;

    /**
     * @brief Renders sprites in batches, with optional render priority.
     *
     * @remark Currently there are 4 texture slots loaded at once.
     * @remark Keeping to 4 textures is most efficient (1 draw call), atlas textures are useful for this.
     * @remark RenderPriority of Sprites may lead to inefficient batching across textures.
     * Try to keep similar RenderPriority within the same texture/ group of 4 textures.
     */
    class SpriteRenderer : public PriorityStore<Sprite>
    {

    public:

        /**
         * @brief Largest number of concurrent textures bound for one batch.
         *
         */
        static const uint8_t MAX_BATCH_BOUND_TEXTURES = MAX_SPRITE_BATCH_BOUND_TEXTURES;

        /**
         * @brief Construct a new SpriteRenderer.
         *
         * @param sizeHint Reserve some memory for this many Sprites.
         */
        SpriteRenderer(size_t sizeHint = 8)
        : PriorityStore(sizeHint)
        {}

        Sprite & getSprite(SpriteId id){ return this->operator[](id); }

        /**
         * @brief Draw with overriding render priority and shader.
         *
         * @param shader An OpenGL Shader to draw all the Sprites with.
         * @param ids Render priorities for the Sprites.
         * @remark Overriding priorities with many sprites performs poorly on cpu.
         * Consider SpriteRenderer::updatePriority to cache priority.
         */
        virtual void draw
        (
            std::shared_ptr<Shader> shader,
            std::multimap<RenderPriority, SpriteId> ids
        )
        {
            std::vector<std::pair<Info, Sprite>> sprites = vectorise(ids);
            draw(shader, sprites);
        }

        /**
         * @brief Draw with overriding render priority.
         *
         * @param ids Render priorities for the Sprites.
         * @remark Overriding priorities with many sprites performs poorly on cpu.
         * Consider SpritRenderer::updatePriority to cache priority.
         */
        virtual void draw(std::multimap<RenderPriority, SpriteId> ids)
        {
            std::vector<std::pair<Info, Sprite>> sprites = vectorise(ids);
            draw(shader, sprites);
        }

        /**
         * @brief Draw with overriding shader.
         *
         * @param shader A Shader to draw all the Sprites with.
         */
        virtual void draw(std::shared_ptr<Shader> shader)
        {
            draw(shader, cache);
        }

        /**
         * @brief Draw with default shader and priority.
         *
         */
        virtual void draw() { draw(shader, cache); }

        virtual void setProjection(glm::mat4 p) {projection = p;}

    protected:

        virtual void draw
        (
            std::shared_ptr<Shader> shader,
            std::vector<std::pair<Info, Sprite>> & sprites
        ) = 0;

        std::shared_ptr<Shader> shader;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SPRITERENDERER_H */
