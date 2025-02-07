#ifndef CELL_H
#define CELL_H

#include <array>

#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Shader/glShader.h>

#include <glUtils.h>

class Cell
{

public:

    Cell(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    : a(a), b(b), c(c)
    {
        generateFaces();
        shader = std::make_unique<jGL::GL::glShader>(vertexShader, fragmentShader);
        shader->use();
        shader->setUniform<glm::vec4>("colour", {0.0,1.0,0.0,0.5});
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_vertices);
        glGenBuffers(1, &a_coords);

        glBindVertexArray(vao);

            createBuffer
            (
                a_vertices,
                cube.data(),
                cube.size(),
                GL_DYNAMIC_DRAW,
                0,
                3,
                0
            );

            createBuffer
            (
                a_coords,
                textureCoords.data(),
                textureCoords.size(),
                GL_STATIC_DRAW,
                1,
                2,
                0
            );

        glBindVertexArray(0);
    }

    ~Cell()
    {
        glDeleteBuffers(1, &a_vertices);
        glDeleteBuffers(1, &a_coords);
        glDeleteVertexArrays(1, &vao);
    }

    void setProjectionView(glm::mat4 pv)
    {
        shader->use();
        shader->setUniform<glm::mat4>("proj", pv);
    }

    void setVectors(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        this->a = a; this->b = b; this->c = c;
        generateFaces();
        glBindVertexArray(vao);

            subFullBuffer
            (
                a_vertices,
                cube.data(),
                cube.size()
            );

        glBindVertexArray(0);
    }

    void draw()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        shader->use();
        glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 12*3);
        glBindVertexArray(0);
    }

private:

    glm::vec3 a, b, c;

    std::unique_ptr<jGL::GL::glShader> shader;

    std::array<float, 12*3*3> cube;

    const std::array<float, 12*3*2> textureCoords =
    {
        0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 0.0, 1.0, 1.0,

        0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0, 1.0, 0.0,

        0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0, 0.0, 1.0,

        0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0, 0.0, 1.0,

        0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0, 0.0, 1.0,

        1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0
    };

    GLuint vao, a_vertices, a_coords;

    const char * vertexShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec3 a_vertices;\n"
        "layout(location=1) in vec2 a_coords;\n"
        "uniform mat4 proj;\n"
        "out vec2 coords;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = proj*vec4(a_vertices.xyz, 1.0);\n"
        "    coords = a_coords;\n"
        "}";

    const char * fragmentShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "uniform vec4 colour;\n"
        "in vec2 coords;\n"
        "out vec4 o_colour;\n"
        "void main()\n"
        "{\n"
        "    float d = 1.0-2.0*min(min(coords.x, 1.0-coords.x), min(coords.y, 1.0-coords.y));\n"
        "    if (d < 0.98) { d = smoothstep(0.97, 0.98, d); }\n"
        "    else { d = 1.0; }\n"
        "    o_colour = vec4(1.0, 0.0, 0.0, d);\n"
        "}";

    void generateFaces()
    {
        // Front vertices.
        glm::vec3 f0 = {0.0, 0.0, 0.0};
        glm::vec3 f1 = f0 + a;
        glm::vec3 f2 = f0 + b;
        glm::vec3 f3 = f0 + a + b;
        // Back vertices.
        glm::vec3 b0 = f0 + c;
        glm::vec3 b1 = b0 + a;
        glm::vec3 b2 = b0 + b;
        glm::vec3 b3 = b0 + a + b;

        std::array<glm::vec3, 12*3> vertices =
        {
            f0, f2, f1,
            f1, f2, f3,

            b0, b1, b2,
            b1, b3, b2,

            f1, f3, b3,
            f1, b3, b1,

            f0, b0, b2,
            f0, b2, f2,

            f0, f1, b1,
            f0, b1, b0,

            f2, b3, f3,
            f2, b2, b3
        };

        glm::vec3 com = {0.0, 0.0, 0.0};
        for (const auto & v : vertices) { com += v; }
        com /= float(vertices.size());

        for (uint8_t i = 0; i < vertices.size(); i++)
        {
            vertices[i] -= com;
            for (uint8_t j = 0; j < 3; j++)
            {
                cube[i*3+j] = vertices[i][j];
            }
        }
    }
};

#endif /* CELL_H */
