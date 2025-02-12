#ifndef ATOMRENDERER_H
#define ATOMRENDERER_H

#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Shader/glShader.h>

#include <hierarchicalTriangularMesh.h>

#include <glUtils.h>
#include <atom.h>
#include <camera.h>

/**
 * @brief Render atoms as sphere meshes.
 *
 * @remark @see HierarchicalTriangularMesh.
 * @remark The level of detail can be automatically de-scaled with distance.
 * @remark The level of detail can be overriden.
 */
class AtomRenderer
{
public:

    /**
     * @brief Construct a new AtomRenderer from a BASE_MESH type.
     *
     * @remark BASE_MESH::ANY will sample from refinements of all BASE_MESH types. Giving a finer range of level of detail .
     * @remark Choosing a specific BASE_MESH will mean the level of detail is refinements of just that mesh.
     * @param atoms the atoms to draw.
     * @param levelOfDetail the level of detail.
     * @param cameraPosition the cartesian position of the camera.
     * @param mesh the base mesh type. @see BASE_MESH.
     */
    AtomRenderer
    (
        const std::vector<Atom> & atoms,
        uint8_t levelOfDetail = 0,
        glm::vec3 cameraPosition = glm::vec3(0),
        BASE_MESH mesh = BASE_MESH::ANY
    )
    {
        meshShader = std::make_unique<jGL::GL::glShader>(meshVertexShader, meshFragmentShader);
        imposterShader = std::make_unique<jGL::GL::glShader>(imposterVertexShader, imposterFragmentShader);
        imposterShader->use();
        imposterShader->setUniform<float>("clipCorrection", 1.5f);
        imposterShader->setUniform<glm::vec4>("lightColour", glm::vec4(1.0f,1.0f,1.0f,1.0f));
        imposterShader->setUniform<float>("ambientLight", 0.1f);

        meshShader->use();
        meshShader->setUniform<glm::vec4>("lightColour", glm::vec4(1.0f,1.0f,1.0f,1.0f));
        meshShader->setUniform<float>("ambientLight", 0.1f);

        if (mesh == BASE_MESH::ANY)
        {
            std::vector<HierarchicalTriangularMesh<float>> htms;

            // Non-regular triangular faces not yet supported.
            for (BASE_MESH m : {BASE_MESH::CUBE, BASE_MESH::DODECAHEDRON})
            {
                HierarchicalTriangularMesh<float> htm(m);
                htm.build(0);
                htms.push_back(htm);
            }

            for (BASE_MESH m : {BASE_MESH::TETRAHEDRON, BASE_MESH::OCTAHEDRON, BASE_MESH::ICOSAHEDRON})
            {
                for (uint8_t i = 0; i < 7; i++)
                {
                    HierarchicalTriangularMesh<float> htm(m);
                    htm.build(i);
                    htms.push_back(htm);
                }
            }

            std::sort(htms.begin(), htms.end());

            for (uint8_t i = 0; i < htms.size(); i++)
            {
                meshes.insert({i, {htms[i].vertices(), htms[i].vertexNormals()}});
                triangleCounts.push_back(htms[i].triangles());
            }
        }
        else
        {
            for (uint8_t i = 0; i < 7; i++)
            {
                HierarchicalTriangularMesh<float> htm(mesh);
                htm.build(i);
                meshes.insert({i, {htm.vertices(), htm.vertexNormals()}});
                triangleCounts.push_back(htm.triangles());
            }
        }

        buffer = std::make_unique<AtomBuffer>
        (
            meshes,
            atoms.size(),
            levelOfDetail
        );

        setLevelOfDetail(levelOfDetail);
        this->cameraPosition = cameraPosition;
        cameraDistances.resize(atoms.size());

        updateAtoms(atoms);
        setAtomScale(1.0f);

        jGL::GL::glError("AtomRenderer::AtomRenderer");
    }

    /**
     * @brief The number of triangles drawn.
     *
     * @param imposters draw with impostor spheres inplace of meshes.
     * @return uint32_t the number of triangles.
     */
    uint32_t triangles(bool impostor = true) const
    {
        uint64_t triangles = 0;
        if (impostor)
        {
            triangles += buffer->atomCount()*2;
        }
        else
        {
            triangles += buffer->atomCount()*triangleCounts[levelOfDetail];
        }
        return triangles;
    }

    /**
     * @brief Set the current level of detail.
     *
     * @param lod the new level of detail.
     */
    void setLevelOfDetail(uint8_t lod){ levelOfDetail = std::min(lod, uint8_t(meshes.size()-1)); }

    /**
     * @brief Get the current level of detail.
     *
     * @return uint8_t the current level of detail.
     */
    uint8_t getLevelOfDetail() const { return levelOfDetail; }

    /**
     * @brief Get the maximum level of detail.
     *
     * @return uint8_t the maximum level of detail.
     */
    uint8_t maxLevelOfDetail() const { return meshes.size()-1; }

    /**
     * @brief Update buffers with new Atom data.
     *
     * @remark will upload data to the GPU.
     * @param atoms the new Atom data to upload.
     */
    void updateAtoms(const std::vector<Atom> & atoms)
    {
        buffer->flip();
        buffer->insert(atoms);
        buffer->updateVertexArray();
    }

    /**
     * @brief Draw the current Atoms
     *
     *  @param imposters draw with impostor spheres inplace of meshes.
     */
    void draw(bool imposters = true)
    {
        if (imposters)
        {
            imposterShader->use();
        }
        else
        {
            meshShader->use();
        }
        buffer->draw(imposters);
        jGL::GL::glError("AtomRenderer::draw");
    }

    /**
     * @brief Set the view matrix.
     *
     * @param v the view matrix.
     */
    void setView(glm::mat4 v) { view = v; setProjectionView(); }

    /**
     * @brief Set the Projection matrix.
     *
     * @param p the projection matrix.
     */
    void setProjection(glm::mat4 p) { projection = p; setProjectionView(); }

    /**
     * @brief Set the lighting of the scene.
     *
     * @param position the camera/light position.
     * @param colour the light colour.
     * @param ambient the ambient light strength.
     */
    void setLighting
    (
        glm::vec3 position,
        glm::vec3 colour = {1.0f, 1.0f, 1.0f},
        float ambient = 0.1f
    )
    {
        cameraPosition = position;
        meshShader->use();
        meshShader->setUniform<glm::vec4>("lightPos", glm::vec4(position, 1.0f));
        meshShader->setUniform<glm::vec4>("lightColour", glm::vec4(colour, 1.0f));
        meshShader->setUniform<float>("ambientLight", ambient);
        imposterShader->use();
        imposterShader->setUniform<glm::vec4>("lightPos", glm::vec4(position, 1.0f));
        imposterShader->setUniform<glm::vec4>("lightColour", glm::vec4(colour, 1.0f));
        imposterShader->setUniform<float>("ambientLight", ambient);
    }

    /**
     * @brief Update shaders from a Camera.
     *
     * @param camera the camera to update from.
     */
    void updateCamera(const Camera & camera)
    {
        cameraPosition = camera.position();
        meshShader->use();
        meshShader->setUniform<glm::vec4>("lightPos", glm::vec4(cameraPosition, 1.0f));
        imposterShader->use();
        imposterShader->setUniform<glm::vec4>("lightPos", glm::vec4(cameraPosition, 1.0f));
        setView(camera.getView());
        setProjection(camera.getProjection());
    }

    /**
     * @brief Set the global atom scaling factor.
     *
     * @param s the new scaling factor.
     */
    void setAtomScale(float s)
    {
        meshShader->use();
        meshShader->setUniform<float>("scaling", s);
        imposterShader->use();
        imposterShader->setUniform<float>("scaling", s);
    }

private:

    std::unique_ptr<jGL::GL::glShader> meshShader, imposterShader;
    glm::vec3 cameraPosition;
    uint8_t levelOfDetail;

    struct SphereMesh { std::vector<float> vertices; std::vector<float> normals; };

    std::map<uint8_t, SphereMesh> meshes;

    uint64_t atomCount = 0;
    std::vector<uint32_t> triangleCounts;

    std::vector<float> cameraDistances;

    glm::mat4 view, projection;

    const char * meshVertexShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec3 a_vertices;\n"
        "layout(location=1) in vec3 a_normals;\n"
        "layout(location=2) in vec4 a_positionsAndScales;\n"
        "layout(location=3) in vec4 a_colours;\n"
        "uniform mat4 proj;\n"
        "uniform float scaling;\n"
        "out vec4 o_colour;\n"
        "out vec3 o_normal;\n"
        "out vec3 fragPos;\n"
        ""
        "void main()\n"
        "{\n"
        "    fragPos = vec3(a_vertices*a_positionsAndScales.w*scaling+a_positionsAndScales.xyz);\n"
        "    gl_Position = proj*vec4(fragPos.xyz, 1.0);\n"
        "    o_colour = a_colours;\n"
        "    o_normal = a_normals;\n"
        "}";

    const char * meshFragmentShader =
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

    const char * imposterVertexShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec2 a_vertices;\n"
        "layout(location=1) in vec4 a_positionsAndScales;\n"
        "layout(location=2) in vec4 a_colours;\n"
        "out vec2 billboard;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform float clipCorrection;\n"
        "uniform float scaling;\n"
        "out vec4 atomPosScale;\n"
        "out vec3 atomViewPos;\n"
        "out vec4 o_colour;\n"
        "void main()\n"
        "{\n"
        "    billboard = a_vertices * clipCorrection;\n"
        "    atomViewPos = (view * vec4(a_positionsAndScales.xyz, 1.0)).xyz;"
        "    gl_Position = proj * (vec4(atomViewPos, 1.0)+vec4(scaling*a_positionsAndScales.w * a_vertices * clipCorrection, 0.0, 1.0));"
        "    atomPosScale = a_positionsAndScales;\n"
        "    atomPosScale.w *= scaling;\n"
        "    o_colour = a_colours;\n"
        "}";

    const char * imposterFragmentShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "in vec2 billboard;\n"
        "in vec3 atomViewPos;\n"
        "in vec4 atomPosScale;\n"
        "in vec4 o_colour;\n"
        "out vec4 colour;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform vec4 lightPos;\n"
        "uniform vec4 lightColour;\n"
        "uniform float ambientLight;\n"
        "bool sphereHit(vec3 rayDirection, vec3 centre, float radius, out vec3 pos, out vec3 normal)\n"
        "{\n"
        "    float b = 2.0 * dot(rayDirection, -centre);\n"
        "    float r2 = radius*radius;\n"
        "    float determinant = b * b - (4.0 * (dot(centre, centre) - r2));\n"
        "    if(determinant < 0.0) { return false; }\n"
        "    determinant = sqrt(determinant);\n"
        "    pos = rayDirection * min((-b+determinant)*0.5, (-b-determinant)*0.5);\n"
        "    normal = normalize(pos - centre);\n"
        "    return true;\n"
        "}\n"
        "void main()\n"
        "{\n"
        "    vec3 lightViewPos = (view*lightPos).xyz;\n"
        "    vec3 rayDirection = normalize(vec3(billboard * atomPosScale.w, 0.0) + atomViewPos);"
        "    vec3 viewNormal; vec3 viewPos;\n"
        "    bool hit = sphereHit(rayDirection, atomViewPos, atomPosScale.w, viewPos, viewNormal);\n"
        "    if (!hit) { discard; }\n"
        "    vec4 clipPos = proj * vec4(viewPos, 1.0);\n"
        "    float ndcDepth = clipPos.z / clipPos.w;\n"
        "    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;\n"
        "    float diff = max(dot(normalize(viewNormal), normalize(lightViewPos-atomViewPos)), 0.0);\n"
        "    colour = vec4((ambientLight + diff)*lightColour.rgb * o_colour.rgb, o_colour.a);\n"
        "}";

    void setProjectionView()
    {
        meshShader->setUniform<glm::mat4>("proj", projection*view);
        imposterShader->setUniform<glm::mat4>("view", view);
        imposterShader->setUniform<glm::mat4>("proj", projection);
    }

    /**
     * @brief Manages OpenGL arrays for Atoms
     * @remark is constructed with a set maximum number of atoms.
     * @remark Atoms are instance rendered with a single mesh.
     */
    struct AtomBuffer
    {
        /**
         * @brief Construct a new AtomBuffer.
         *
         * @param mesh the mesh to instance draw with.
         * @param atoms the maximum number of atoms in this buffer.
         */
        AtomBuffer
        (
            std::map<uint8_t, SphereMesh> meshes,
            uint32_t atoms,
            uint8_t levelOfDetail
        )
        : meshes(meshes), size(atoms), levelOfDetail(std::min(meshes.size()-1, size_t(levelOfDetail)))
        {
            glGenVertexArrays(1, &vao_mesh);
            glGenVertexArrays(1, &vao_imposter);
            glGenBuffers(1, &a_quad);
            glGenBuffers(1, &a_colours);
            glGenBuffers(1, &a_positionsAndScales);

            a_meshVertices = std::vector<GLuint>(meshes.size(), 0);
            a_meshNormals = std::vector<GLuint>(meshes.size(), 0);
            glGenBuffers(a_meshVertices.size(), a_meshVertices.data());
            glGenBuffers(a_meshNormals.size(), a_meshNormals.data());

            positionsAndScales.resize(size*4);
            colours.resize(size*4);

            glBindVertexArray(vao_mesh);

                createBuffer
                (
                    a_meshVertices[this->levelOfDetail],
                    meshes[this->levelOfDetail].vertices.data(),
                    meshes[this->levelOfDetail].vertices.size(),
                    GL_STATIC_DRAW,
                    0,
                    3,
                    0
                );

                createBuffer
                (
                    a_meshNormals[this->levelOfDetail],
                    meshes[this->levelOfDetail].normals.data(),
                    meshes[this->levelOfDetail].normals.size(),
                    GL_STATIC_DRAW,
                    1,
                    3,
                    0
                );

                createBuffer
                (
                    a_positionsAndScales,
                    positionsAndScales.data(),
                    positionsAndScales.size(),
                    GL_DYNAMIC_DRAW,
                    2,
                    4,
                    1
                );

                createBuffer
                (
                    a_colours,
                    colours.data(),
                    colours.size(),
                    GL_DYNAMIC_DRAW,
                    3,
                    4,
                    1
                );

            glBindVertexArray(0);

            glBindVertexArray(vao_imposter);

                createBuffer
                (
                    a_quad,
                    quad.data(),
                    quad.size(),
                    GL_STATIC_DRAW,
                    0,
                    2,
                    0
                );

                enableBuffer
                (
                    a_positionsAndScales,
                    1,
                    4,
                    1
                );

                enableBuffer
                (
                    a_colours,
                    2,
                    4,
                    1
                );

            glBindVertexArray(0);
        }

        ~AtomBuffer()
        {
            glDeleteBuffers(a_meshVertices.size(), a_meshVertices.data());
            glDeleteBuffers(a_meshNormals.size(), a_meshNormals.data());
            glDeleteBuffers(1, &a_colours);
            glDeleteBuffers(1, &a_positionsAndScales);
            glDeleteBuffers(1, &a_quad);
            glDeleteVertexArrays(1, &vao_mesh);
            glDeleteVertexArrays(1, &vao_imposter);
        }

        /**
         * @brief Set the buffer position to the start.
         *
         */
        void flip() { index = 0; atoms = 0; }

        /**
         * @brief Insert (update) an Atoms data.
         *
         * @param atom the Atom data to insert.
         */
        void insert(const Atom & atom)
        {
            positionsAndScales[index] = atom.position.x;
            positionsAndScales[index+1] = atom.position.y;
            positionsAndScales[index+2] = atom.position.z;
            positionsAndScales[index+3] = atom.scale;

            colours[index] = atom.colour.r;
            colours[index+1] = atom.colour.g;
            colours[index+2] = atom.colour.b;
            colours[index+3] = atom.colour.a;
            index += 4;
            atoms++;
        }

        /**
         * @brief The maximum number of Atoms to draw.
         *
         * @return uint32_t number of atoms.
         */
        uint32_t atomCount() const { return atoms; }

        /**
         * @brief Draw the atoms.
         *
         * @param count override number of atoms.
         * @param imposters draw with impostor spheres inplace of meshes.
         */
        void draw(uint32_t count, bool imposters)
        {
            count = std::min(count, atoms);
            if (count == 0) { return; }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            if (imposters) {
                glFrontFace(GL_CW);
                glBindVertexArray(vao_imposter);

                    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);

                glBindVertexArray(0);
                glFrontFace(GL_CCW);
            }
            else
            {
                glBindVertexArray(vao_mesh);

                    glDrawArraysInstanced(GL_TRIANGLES, 0, meshes[levelOfDetail].vertices.size(), count);

                glBindVertexArray(0);
            }
        }

        /**
         * @brief Set the level of detail, selecting the mesh to draw.
         *
         * @param levelOfDetail the new level of detail.
         */
        void setLevelOfDetail(uint8_t levelOfDetail)
        {

            this->levelOfDetail = std::min(size_t(levelOfDetail), meshes.size()-1);
            glBindVertexArray(vao_mesh);
                createBuffer
                (
                    a_meshVertices[this->levelOfDetail],
                    meshes[this->levelOfDetail].vertices.data(),
                    meshes[this->levelOfDetail].vertices.size(),
                    GL_STATIC_DRAW,
                    0,
                    3,
                    0
                );

                createBuffer
                (
                    a_meshNormals[this->levelOfDetail],
                    meshes[this->levelOfDetail].normals.data(),
                    meshes[this->levelOfDetail].normals.size(),
                    GL_STATIC_DRAW,
                    1,
                    3,
                    0
                );
            glBindVertexArray(0);
        }

        /**
         * @brief Draw all the Atoms.
         *
         * @param imposters draw with impostor spheres inplace of meshes.
         */
        void draw(bool imposters = true) { draw(atoms, imposters); }

        /**
         * @brief Insert a batch of Atoms.
         *
         * @param atoms the batch of Atoms to insert.
         */
        void insert(const std::vector<Atom> & atoms)
        {
            flip();
            for (const Atom & atom : atoms) { insert(atom); }
        }

        /**
         * @brief Upload Atom data to the GPU.
         *
         */
        void updateVertexArray()
        {
            glBindVertexArray(vao_mesh);

                subFullBuffer(a_positionsAndScales, positionsAndScales.data(), positionsAndScales.size());
                subFullBuffer(a_colours, colours.data(), colours.size());

            glBindVertexArray(0);
        }

    private:

        std::map<uint8_t, SphereMesh> meshes;
        uint32_t size;
        uint8_t levelOfDetail;
        GLuint vao_mesh, vao_imposter, a_quad, a_positionsAndScales, a_colours;
        std::vector<GLuint> a_meshVertices, a_meshNormals;
        std::vector<float> positionsAndScales;
        std::vector<float> colours;

        uint32_t index = 0;
        uint32_t atoms = 0;

        const std::array<float, 8> quad =
        {
            -1.0,-1.0,
            -1.0,1.0,
            1.0,-1.0,
            1.0,1.0
        };
    };

    std::unique_ptr<AtomBuffer> buffer;
};

#endif /* ATOMRENDERER_H */
