#ifndef GLSHAPERENDERER_H
#define GLSHAPERENDERER_H

#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/OpenGL/gl.h>
#include <jGL/shapeRenderer.h>

namespace jGL::GL
{
    /**
     * @brief OpenGL implementation of ShapeRenderer.
     *
     */
    class glShapeRenderer : public ShapeRenderer
    {

    public:

        /**
         * @brief Construct a new glShapeRenderer.
         *
         * @param sizeHint hint at the number of shapes.
         */
        glShapeRenderer(size_t sizeHint = 8)
        : ShapeRenderer(sizeHint)
        {
            xytheta = std::vector<float>(sizeHint*xythetaDim+padShapes*xythetaDim,0.0f);
            scale = std::vector<float>(sizeHint*scaleDim+padShapes*scaleDim,0.0f);
            colours = std::vector<float>(sizeHint*coloursDim+padShapes*coloursDim,0.0f);
            initGL();
            shader = std::make_shared<glShader>(shapeVertexShader, rectangleFragmentShader);
            shader->use();
        }

        ~glShapeRenderer()
        {
            freeGL();
        }

        /**
         * @brief A vertex shader for any default shapes.
         *
         */
        static const char * shapeVertexShader;

        /**
         * @brief A fragment shader to draw rectangles.
         * @remark Assume shapeVertexShader inputs
         */
        static const char * rectangleFragmentShader;

        /**
         * @brief A fragment shader to draw ellipses.
         * @remark Assume shapeVertexShader inputs
         */
        static const char * ellipseFragmentShader;

    private:

        void draw
        (
            std::shared_ptr<Shader> shader,
            std::vector<std::pair<Info, Shape>> & shapes,
            UpdateInfo info = UpdateInfo()
        );

        GLuint vao, a_position, a_xytheta, a_scale, a_colour;

        float quad[6*4] =
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,   // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,    // top left
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            0.5f,  0.5f, 1.0f, 1.0f  // top right
        };

        std::vector<float> xytheta;
        size_t xythetaDim = 3;
        size_t xythetaAttribtue = 1;

        std::vector<float> scale;
        size_t scaleDim = 2;
        size_t scaleAttribtue = 2;

        std::vector<float> colours;
        size_t coloursDim = 4;
        size_t coloursAttribtue = 3;

        size_t padShapes = 8;

        void initGL();
        void freeGL();

    };
}

#endif /* GLSHAPERENDERER_H */
