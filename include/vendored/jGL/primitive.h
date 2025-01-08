#ifndef PRIMITIVE
#define PRIMITIVE

#include <glm/glm.hpp>
#include <array>
#include <iostream>

namespace jGL
{

    /** 
     * @brief Higher priority is drawn last.
     * @typedef RenderPriority 
     * */
    typedef uint64_t RenderPriority;

    struct Primitive
    {
        virtual ~Primitive() = default;
    };

    /**
     * @brief Particle with a texture, with a position, orientation, and scale, colour, and texture region.
     * 
     */
    struct TexturedParticle : public Primitive
    {
        TexturedParticle() = default;
        
        TexturedParticle(glm::vec4 p, glm::vec4 c, glm::vec4 t)
        : state(p), colour(c), texCoord(t)
        {}

        glm::vec4 state;
        glm::vec4 colour;
        glm::vec4 texCoord;
    };

    /**
     * @brief Position, rotation, and scale.
     * 
     */
    struct Transform : public Primitive
    {

        Transform(double x, double y, double t, double s)
        : x(x), y(y), theta(t), scaleX(s), scaleY(s)
        {}

        Transform(double x, double y, double t, double sx, double sy)
        : x(x), y(y), theta(t), scaleX(sx), scaleY(sy)
        {}

        Transform()
        : x(0.0), y(0.0), theta(0.0), scaleX(0.0), scaleY(0.0)
        {}

        double x;
        double y;
        double theta;
        double scaleX;
        double scaleY;
    };

    /**
     * @brief Rectangular region of a texture in pixels.
     * 
     * @param tx Lower left corner of region, along width.
     * @param ty Lower left corner of region, along height.
     * @param lx Length of region along width.
     * @param ly Length of region along height.
     * 
     * @remark 0 lx and ly will be handled as the maximum lengths in e.g.
     * SpriteRenderer.
     */
    struct TextureRegion : public Primitive
    {

        TextureRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
        : tx(x), ty(y), lx(w), ly(h)
        {}

        TextureRegion()
        : tx(0), ty(0), lx(0), ly(0)
        {}

        uint16_t tx;
        uint16_t ty;
        uint16_t lx;
        uint16_t ly;
    };

     /**
     * @brief Rectangular region of a texture, normalised by the textures dimensions.
     *
     * @param tx Lower left corner of region, along width, in [0, 1].
     * @param ty Lower left corner of region, along height, in [0, 1].
     * @param lx Length of region along width, in [0, 1].
     * @param ly Length of region along height, in [0, 1].
     *
     * @remark Values are normalised to a texture width and height.
     * @remark 0 lx and ly will be handled as the maximum lengths in e.g.
     * SpriteRenderer.
     */
    struct NormalisedTextureRegion : public Primitive
    {

        NormalisedTextureRegion(float x, float y, float w, float h)
        : tx(x), ty(y), lx(w), ly(h)
        {}

        NormalisedTextureRegion()
        : tx(0), ty(0), lx(0), ly(0)
        {}
        
        float tx; 
        float ty;
        float lx;
        float ly;
    };

    /**
     * @brief A bounding box template.
     * @remark See WorldBoundingBox and ScreenBoundingBox typedefs
     */
    template <class T>
    struct BoundingBox 
    {
        BoundingBox()
        : vertices({glm::tvec2<T>(0),glm::tvec2<T>(0),glm::tvec2<T>(0),glm::tvec2<T>(0)})
        {}

        BoundingBox(const std::array<glm::tvec2<T>, 4> & v)
        : vertices(v)
        {}
        
        /**
         * @brief Vertices of bounding box.
         * @remark Assumed in anti-clockwise order, e.g. Sprite::getWorldBoundingBox
         */
        std::array<glm::tvec2<T>, 4> vertices;

        /**
         * @brief If the point (x, y) is in the box.
         * 
         * @param x Point x coordinate.
         * @param y Point y coordinate.
         * @return true (x, y) is in the box.
         * @return false (x, y) is not in the box.
         * @remark static_cast<float>s (x, y) for checking.
         */
        bool in(T x, T y)
        {
            auto handedness = [](float x, float y, float x0, float y0, float x1, float y1)
            {
                return (y1-y0)*x+(x0-x1)*y+(x1*y0-y1*x0) >= 0;
            };
            float px = static_cast<float>(x);
            float py = static_cast<float>(y);
            bool h = handedness(px, py, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y);

            return (handedness(px, py, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y) == h &&
                   handedness(px, py, vertices[2].x, vertices[2].y, vertices[3].x, vertices[3].y) == h &&
                   handedness(px, py, vertices[3].x, vertices[3].y, vertices[0].x, vertices[0].y) == h);
        }
    };

    /**
     * @brief A world space bounding box.
     * 
     */
    class WorldBoundingBox : public BoundingBox<float>
    {
    public:
        WorldBoundingBox()
        : BoundingBox<float>({glm::tvec2<float>(0),glm::tvec2<float>(0),glm::tvec2<float>(0),glm::tvec2<float>(0)})
        {}

        WorldBoundingBox(const std::array<glm::tvec2<float>, 4> & v)
        : BoundingBox<float>(v)
        {}

        /**
         * @brief Returns a Transform for ShapeRenderer.
         * 
         * @return Transform 
         */
        Transform toTransform()
        {
            float lowest = vertices[0].y; uint8_t lowestVertex = 0;
            glm::vec2 centre = vertices[0];
            for (uint8_t i = 1; i < vertices.size(); i++)
            {
                if (vertices[i].y < lowest) { lowestVertex = i; lowest = vertices[i].y; }
                centre += vertices[i];
            }
            centre *= 0.25;

            uint8_t next = (lowestVertex + 1) % vertices.size();
            glm::vec2 r = vertices[next] - vertices[lowestVertex];
            float d = std::sqrt(r.x*r.x+r.y*r.y);
            glm::vec2 n = r / d;
            float theta = 0.0;
            if (n.y != 0.0)
            {
                theta = std::atan2(n.y, n.x);
            }

            r = vertices[(lowestVertex - 1) % vertices.size()] - vertices[lowestVertex];

            return Transform(centre.x, centre.y, -theta, d, std::sqrt(r.x*r.x+r.y*r.y));
        }
    };

    /**
     * @brief A screen space bounding box.
     * 
     */
    typedef BoundingBox<uint16_t> ScreenBoundingBox;
}

/**
 * @brief Prints each vertex on a new line.
 * 
 * @param out 
 * @param sbb 
 * @return std::ostream& 
 */
std::ostream & operator <<(std::ostream & out, const jGL::ScreenBoundingBox & sbb);

/**
 * @brief Prints each vertex on a new line.
 * 
 * @param out 
 * @param wbb 
 * @return std::ostream& 
 */
std::ostream & operator <<(std::ostream & out, const jGL::WorldBoundingBox & wbb);
#endif /* PRIMITIVE */
