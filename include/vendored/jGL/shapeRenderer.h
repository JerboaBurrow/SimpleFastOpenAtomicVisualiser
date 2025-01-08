#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include <jGL/shape.h>
#include <jGL/shader.h>
#include <jGL/priorityStore.h>

#include <unordered_map>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

#include <chrono>

namespace jGL
{

    /**
     * @brief User name for a Shape.
     * @typedef SpriteId
     * */
    typedef std::string ShapeId;

    /**
     * @brief Renders shapes with optional rendering priority.
     * @remark Currently Circle and Rectangle are defined as shaders.
     */
    class ShapeRenderer : public PriorityStore<Shape>
    {

    public:

        /**
         * @brief Control updated data for drawing.
         *
         */
        struct UpdateInfo
        {
            UpdateInfo()
            : xytheta(true), scale(true), colour(true)
            {}

            bool xytheta, scale, colour;
        };

        /**
         * @brief Construct a new ShapeRenderer
         *
         * @param sizeHint Hint at the number of shapes
         */
        ShapeRenderer(size_t sizeHint = 8)
        : PriorityStore(sizeHint), shader(nullptr)
        {}

        /**
         * @brief Construct a new ShapeRenderer
         *
         * @param shader The default shader to use
         * @param sizeHint Hint at the number of shapes
         */
        ShapeRenderer(std::shared_ptr<Shader> shader, size_t sizeHint = 8)
        : PriorityStore(sizeHint), shader(shader)
        {}

        Shape & getShape(ShapeId id) { return this->operator[](id); }

        const Transform * getTransform(ShapeId id) { return getShape(id).transform; }
        const glm::vec4 * getColour(ShapeId id) { return getShape(id).colour; }

        /**
         * @brief Draw with overriding render priority and shader.
         *
         * @param shader A Shader to draw all the Sprites with.
         * @param ids Render priorities for the Sprites.
         * @remark Only ShapeIds appearing in the ids argument will be rendered
         * in this draw call.
         * @remark Overriding priorities with many shapes performs poorly on cpu.
         * Consider ShapeRenderer::updatePriority to cache priority.
         */
        virtual void draw
        (
            std::shared_ptr<Shader> shader,
            std::multimap<RenderPriority,ShapeId> & ids,
            UpdateInfo info = UpdateInfo()
        )
        {
            std::vector<std::pair<Info, Shape>> shapes = vectorise(ids);
            draw(shader, shapes, info);
        }

        /**
         * @brief Draw with overriding render priority.
         *
         * @param ids Render priorities for the Sprites.
         * @remark Only ShapeIds appearing in the ids argument will be rendered
         * in this draw call.
         * @remark Overriding priorities with many shapes performs poorly on cpu.
         * Consider ShapeRenderer::updatePriority to cache priorities.
         */
        virtual void draw
        (
            std::multimap<RenderPriority, ShapeId> & ids,
            UpdateInfo info = UpdateInfo()
        )
        {
            std::vector<std::pair<Info, Shape>> shapes = vectorise(ids);
            draw(shader, shapes, info);
        }

        /**
         * @brief Draw with overriding shader and cached priorities.
         *
         * @param shader A Shader to draw all the Sprites with.
         */
        virtual void draw
        (
            std::shared_ptr<Shader> shader,
            UpdateInfo info = UpdateInfo()
        )
        {
            draw(shader, cache, info);
        }

        /**
         * @brief Draw with default shader and cached priorities.
         *
         */
        virtual void draw
        (
            UpdateInfo info = UpdateInfo()
        )
        {
            draw(shader, cache, info);
        }

        bool hasId(const ShapeId id) const { return idToElement.find(id) != idToElement.end(); }

        virtual void setProjection(glm::mat4 p) {projection = p;}

    protected:

        virtual void draw
        (
            std::shared_ptr<Shader> shader,
            std::vector<std::pair<Info, Shape>> & shapes,
            UpdateInfo info = UpdateInfo()
        ) = 0;

        std::shared_ptr<Shader> shader;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SHAPERENDERER_H */
