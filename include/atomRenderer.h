#ifndef ATOMRENDERER
#define ATOMRENDERER

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <utility>

#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Shader/glShader.h>

#include <HTM.h>

#include <atom.h>

class AtomRenderer
{
public:

    const uint8_t maxLevelOfDetail = 8;

    AtomRenderer
    (
        const std::vector<Atom> & atoms,
        uint8_t levelOfDetail = 0,
        glm::vec3 cameraPosition = glm::vec3(0)
    )
    {
        shader = std::make_unique<jGL::GL::glShader>(vertexShader, fragmentShader);

        for (uint8_t i = 0; i < maxLevelOfDetail; i++)
        {
            HTM<float> htm;
            htm.build(i);
            meshes.insert(
                {
                    i,
                    {
                        htm.vertices(),
                        htm.vertexNormals()
                    }
                }
            );
        }

        setLevelOfDetail(levelOfDetail);
        this->cameraPosition = cameraPosition;
        cameraDistances.resize(atoms.size());

        buffer = std::make_unique<AtomBuffer>(meshes[levelOfDetail], atoms.size());
        bufferLowRes = std::make_unique<AtomBuffer>(meshes[0], atoms.size());
        updateAtoms(atoms);

        jGL::GL::glError("AtomRenderer::AtomRenderer");
    }

    uint32_t triangles() const { return 8 * std::pow(4, levelOfDetail); }

    void setLevelOfDetail(uint8_t lod){ levelOfDetail = std::min(lod, maxLevelOfDetail); }

    uint8_t getLevelOfDetail() const { return levelOfDetail; }

    void updateAtoms(const std::vector<Atom> & atoms)
    {
        uint64_t i = 0;
        uint64_t furthest = 0;
        float maxDistance = 0.0f;
        float minDistance = 1e9;

        for (const Atom & atom : atoms)
        {
            // scalar projection of atom position onto camera vector
            cameraDistances[i] = glm::dot(atom.position-cameraPosition, glm::normalize(-cameraPosition));
            if (cameraDistances[i] > maxDistance) { furthest = i; }
            maxDistance = std::max(maxDistance, cameraDistances[i]);
            minDistance = std::min(minDistance, cameraDistances[i]);
            i++;
        }

        highRes.clear();
        lowRes.clear();
        float rcrit = 0.5f*(maxDistance-minDistance);
        for (uint32_t i = 0; i < atoms.size(); i++)
        {
            float r = glm::length(atoms[furthest].position-atoms[i].position);
            if (r < rcrit)
            {
                lowRes.push_back(atoms[i]);
            }
            else
            {
                highRes.push_back(atoms[i]);
            }
        }
        buffer->update(highRes);
        buffer->updateVertexArray();
        bufferLowRes->update(lowRes);
        bufferLowRes->updateVertexArray();
    }

    void draw()
    {
        shader->use();
        bufferLowRes->draw(lowRes.size());
        buffer->draw(highRes.size());
        jGL::GL::glError("AtomRenderer::draw");
    }

    void setProjection(glm::mat4 p) { shader->use(); shader->setUniform<glm::mat4>("proj", p); }

    void setLighting(glm::vec3 position, glm::vec3 colour, float ambient)
    {
        cameraPosition = position;
        shader->setUniform<glm::vec4>("lightPos", glm::vec4(position, 1.0f));
        shader->setUniform<glm::vec4>("lightColour", glm::vec4(colour, 1.0f));
        shader->setUniform<float>("ambientLight", ambient);
    }

private:

    std::unique_ptr<jGL::GL::glShader> shader;
    glm::vec3 cameraPosition;
    uint8_t levelOfDetail;

    struct SphereMesh { std::vector<float> vertices; std::vector<float> normals; };

    std::map<uint8_t, SphereMesh> meshes;

    uint64_t atomCount = 0;

    std::vector<float> cameraDistances;

    const char * vertexShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec3 a_vertices;\n"
        "layout(location=1) in vec3 a_normals;\n"
        "layout(location=2) in vec4 a_positionsAndScales;\n"
        "layout(location=3) in vec4 a_colours;\n"
        "uniform mat4 proj;\n"
        "out vec4 o_colour;\n"
        "out vec3 o_normal;\n"
        "out vec3 fragPos;\n"
        ""
        "void main()\n"
        "{\n"
        "    fragPos = vec3(a_vertices*a_positionsAndScales.w+a_positionsAndScales.xyz);\n"
        "    gl_Position = proj*vec4(fragPos.xyz, 1.0);\n"
        "    o_colour = a_colours;\n"
        "    o_normal = a_normals;\n"
        "}";

    const char * fragmentShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "uniform vec4 lightPos;\n"
        "uniform vec4 lightColour;\n"
        "uniform float ambientLight;\n"
        "in vec4 o_colour;\n"
        "in vec3 o_normal;\n"
        "in vec3 fragPos;\n"
        "out vec4 colour;\n"
        "void main()\n"
        "{\n"
        "    vec3 lightDir = normalize(lightPos.xyz - fragPos);\n"
        "    float diff = max(dot(normalize(o_normal), lightDir), 0.0);\n"
        "    colour = vec4((ambientLight + diff)*lightColour.rgb * o_colour.rgb, o_colour.a);\n"
        "}";

    struct AtomBuffer
    {
        AtomBuffer
        (
            const SphereMesh & mesh,
            uint32_t atoms
        )
        : mesh(mesh), size(atoms)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &a_vertices);
            glGenBuffers(1, &a_normals);
            glGenBuffers(1, &a_colours);
            glGenBuffers(1, &a_positionsAndScales);

            positionsAndScales.resize(size*4);
            colours.resize(size*4);

            glBindVertexArray(vao);

                glBindBuffer(GL_ARRAY_BUFFER, a_vertices);
                    glBufferData
                    (
                        GL_ARRAY_BUFFER,
                        sizeof(float)*mesh.vertices.size(),
                        &mesh.vertices[0],
                        GL_STATIC_DRAW
                    );
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer
                    (
                        0,
                        3,
                        GL_FLOAT,
                        false,
                        3*sizeof(float),
                        0
                    );
                    // never increment per instance
                    glVertexAttribDivisor(0, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER, a_normals);
                    glBufferData
                    (
                        GL_ARRAY_BUFFER,
                        sizeof(float)*mesh.normals.size(),
                        &mesh.normals[0],
                        GL_STATIC_DRAW
                    );
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer
                    (
                        1,
                        3,
                        GL_FLOAT,
                        false,
                        3*sizeof(float),
                        0
                    );
                    // never increment per instance
                    glVertexAttribDivisor(1, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER, a_positionsAndScales);
                    glBufferData
                    (
                        GL_ARRAY_BUFFER,
                        sizeof(float)*positionsAndScales.size(),
                        &positionsAndScales[0],
                        GL_DYNAMIC_DRAW
                    );
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer
                    (
                        2,
                        4,
                        GL_FLOAT,
                        false,
                        4*sizeof(float),
                        0
                    );
                    glVertexAttribDivisor(2, 1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER, a_colours);
                    glBufferData
                    (
                        GL_ARRAY_BUFFER,
                        sizeof(float)*colours.size(),
                        &colours[0],
                        GL_DYNAMIC_DRAW
                    );
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer
                    (
                        3,
                        4,
                        GL_FLOAT,
                        false,
                        4*sizeof(float),
                        0
                    );
                    glVertexAttribDivisor(3, 1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        ~AtomBuffer()
        {
            glDeleteBuffers(1, &a_vertices);
            glDeleteBuffers(1, &a_normals);
            glDeleteBuffers(1, &a_colours);
            glDeleteBuffers(1, &a_positionsAndScales);
            glDeleteVertexArrays(1, &vao);
        }

        void draw(uint32_t count)
        {
            glBindVertexArray(vao);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vertices.size(), count);

            glBindVertexArray(0);
        }

        void update(const std::vector<Atom> & atoms)
        {
            uint32_t i = 0;
            for (const Atom & atom : atoms)
            {
                positionsAndScales[i] = atom.position.x;
                positionsAndScales[i+1] = atom.position.y;
                positionsAndScales[i+2] = atom.position.z;
                positionsAndScales[i+3] = atom.scale;

                colours[i] = atom.colour.r;
                colours[i+1] = atom.colour.g;
                colours[i+2] = atom.colour.b;
                colours[i+3] = atom.colour.a;
                i+=4;
            }
        }

        void updateVertexArray()
        {
            glBindVertexArray(vao);

                glBindBuffer(GL_ARRAY_BUFFER, a_positionsAndScales);
                    glBufferSubData
                    (
                        GL_ARRAY_BUFFER,
                        0,
                        sizeof(float)*positionsAndScales.size(),
                        &positionsAndScales[0]
                    );
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER, a_colours);
                    glBufferSubData
                    (
                        GL_ARRAY_BUFFER,
                        0,
                        sizeof(float)*colours.size(),
                        &colours[0]
                    );
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        const SphereMesh & mesh;
        uint32_t size;
        GLuint vao, a_vertices, a_normals, a_positionsAndScales, a_colours;
        std::vector<float> positionsAndScales;
        std::vector<float> colours;
    };

    std::unique_ptr<AtomBuffer> buffer;
    std::unique_ptr<AtomBuffer> bufferLowRes;

    std::vector<Atom> highRes;
    std::vector<Atom> lowRes;

};

#endif /* ATOMRENDERER */
