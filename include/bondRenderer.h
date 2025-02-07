#ifndef BONDRENDERER_H
#define BONDRENDERER_H

#include <jGL/OpenGL/gl.h>

#include <glUtils.h>
#include <atom.h>
#include <bond.h>

/**
 * @brief Render Bonds as ray-traced cylinders.
 *
 */
class BondRenderer
{
public:

    /**
     * @brief Construct a new BondRenderer from set bonds and atoms.
     *
     * @param bonds the current Bonds between atoms.
     * @param atoms the Atoms with bonds Bonds.
     * @param maxBonds a hint to the maximum number of Bonds.
     */
    BondRenderer
    (
        const std::vector<Bond> & bonds,
        const std::vector<Atom> & atoms,
        uint64_t maxBonds
    )
    : bonds(0), maxBonds(maxBonds)
    {
        shader = std::make_unique<jGL::GL::glShader>(vertexShader, fragmentShader);
        shader->use();
        shader->setUniform<float>("clipCorrection", 1.75f);
        shader->setUniform<glm::vec4>("lightColour", glm::vec4(1.0f,1.0f,1.0f,1.0f));
        shader->setUniform<float>("ambientLight", 0.1f);
        setBondScale(1.0f);
        init();

        for (const Bond & bond : bonds)
        {
            insert(bond, atoms);
        }

        updateVertexArray();
    }

    ~BondRenderer()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &a_vertices);
        glDeleteBuffers(1, &b_vertices);
        glDeleteBuffers(1, &a_colours);
        glDeleteBuffers(1, &b_colours);
        glDeleteBuffers(1, &a_quad);
    }

    /**
     * @brief Set the clip correction.
     *
     * @remark The clip correction is a fudge factor for
     * the ray tracing. It is the size of the square picture plane
     * through which rays are traced. Due to projective geometry
     * a value of 1.0f leaves part of a sphere/cylinder off plane.
     * Larger values may be needed for thinner/longer cylinders.
     * @param correction overdraw amount.
     */
    void setClipCorrection(float correction)
    {
        shader->use();
        shader->setUniform<float>("clipCorrection", correction);
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
        shader->use();
        shader->setUniform<glm::vec4>("lightPos", glm::vec4(position, 1.0f));
        shader->setUniform<glm::vec4>("lightColour", glm::vec4(colour, 1.0f));
        shader->setUniform<float>("ambientLight", ambient);
    }

    /**
     * @brief Update shaders from a Camera.
     *
     * @param camera the camera to update from.
     */
    void updateCamera(const Camera & camera)
    {
        cameraPosition = camera.position();
        shader->use();
        shader->setUniform<glm::vec4>("lightPos", glm::vec4(cameraPosition, 1.0f));
        setView(camera.getView());
        setProjection(camera.getProjection());
    }

    /**
     * @brief Set the uniform radii of bonds.
     *
     * @param scale the scale (radii) of bonds.
     */
    void setBondScale(float scale)
    {
        shader->use();
        shader->setUniform<float>("bondScale", scale);
    }

    /**
     * @brief The number of triangles drawn.
     *
     * @return uint32_t the number of triangles.
     */
    uint32_t triangles() const
    {
        return bonds*2;
    }

    /**
     * @brief Update the bonds rendered on the GPU.
     *
     * @param bonds the new bonds.
     * @param atoms the new atoms.
     */
    void update
    (
        const std::vector<Bond> & bonds,
        const std::vector<Atom> & atoms
    )
    {
        flip();
        for (const Bond & bond : bonds)
        {
            insert(bond, atoms);
        }
        updateVertexArray();
    }

    /**
     * @brief Draw the bonds.
     *
     * @param count override number of bonds.
     */
    void draw(uint32_t count)
    {
        count = std::min(count, bonds);
        if (count == 0) { return; }

        shader->use();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glFrontFace(GL_CW);
        glBindVertexArray(vao);

            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);

        glBindVertexArray(0);
        glFrontFace(GL_CCW);
    }

    /**
     * @brief Draw all the bonds.
     *
     */
    void draw() { draw(bonds); }


private:

    uint32_t bonds;
    uint32_t maxBonds;
    std::unique_ptr<jGL::GL::glShader> shader;
    glm::vec3 cameraPosition;

    GLuint vao, a_vertices, b_vertices, a_colours, b_colours, a_quad;
    std::vector<float> positionsAAndScale;
    std::vector<float> positionsBAndScale;
    std::vector<float> coloursA;
    std::vector<float> coloursB;

    glm::mat4 view, projection;

    uint32_t index = 0;

    const std::array<float, 8> quad =
    {
        -1.0,-1.0,
        -1.0,1.0,
        1.0,-1.0,
        1.0,1.0
    };

    const char * vertexShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "layout(location=0) in vec2 a_vertices;\n"
        "layout(location=1) in vec4 a_positionsAAndScales;\n"
        "layout(location=2) in vec4 a_positionsBAndScales;\n"
        "layout(location=3) in vec4 a_colours;\n"
        "layout(location=4) in vec4 b_colours;\n"
        "out vec2 billboard;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform float clipCorrection;\n"
        "uniform float bondScale;\n"
        "out vec4 aPosScale;\n"
        "out vec3 aViewPos;\n"
        "out vec4 bPosScale;\n"
        "out vec3 bViewPos;\n"
        "out vec4 a_colour;\n"
        "out vec4 b_colour;\n"
        "out vec3 comViewPos;\n"
        "void main()\n"
        "{\n"
        "    billboard = a_vertices * clipCorrection;\n"
        "    aViewPos = (view * vec4(a_positionsAAndScales.xyz, 1.0)).xyz;"
        "    bViewPos = (view * vec4(a_positionsBAndScales.xyz, 1.0)).xyz;"
        "    comViewPos = (aViewPos+bViewPos)*0.5;\n"
        "    gl_Position = proj * (vec4(comViewPos, 1.0)+vec4(bondScale * a_vertices * clipCorrection, 0.0, 1.0));"
        "    aPosScale = a_positionsAAndScales;\n"
        "    bPosScale = a_positionsBAndScales;\n"
        "    a_colour = a_colours;\n"
        "    b_colour = b_colours;\n"
        "}";

    const char * fragmentShader =
        "#version " GLSL_VERSION "\n"
        "precision lowp float; precision lowp int;\n"
        "in vec2 billboard;\n"
        "in vec3 aViewPos;\n"
        "in vec3 bViewPos;\n"
        "in vec4 aPosScale;\n"
        "in vec4 bPosScale;\n"
        "in vec4 a_colour;\n"
        "in vec4 b_colour;\n"
        "in vec3 comViewPos;\n"
        "out vec4 colour;\n"
        "uniform mat4 view;\n"
        "uniform mat4 proj;\n"
        "uniform vec4 lightPos;\n"
        "uniform vec4 lightColour;\n"
        "uniform float ambientLight;\n"
        "uniform float bondScale;\n"
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
        /*
         * Creates a capsule (sphere integrated along a line).
         *           .-.B
         *         /  / \
         *        /  /  /
         *       /  /  /
         *      /  /  /
         *     /  /  /
         *     \ /  /
         *      A--
         *
         * To derive, consider in view space a ray r(t) = t*v, and a capsule with midline c(s) = a+s*n.
         *  Then solve the quadratic ||a-r(t)-[(a-r(t).n)]n||^2-R^2 = 0 (=> r(t)->midline distance = R);
         *  Also then project the hit point to the midline for normals.
         */
        "bool capsuleHit"
        "("
        "    vec3 rayDirection,"
        "    vec3 capsuleDirection,"
        "    vec3 capsuleA,"
        "    vec3 capsuleB,"
        "    float radius,"
        "    float midLength,"
        "    out vec3 viewPos,"
        "    out vec3 viewNormal,"
        "    out vec3 projectedHitPoint,"
        "    out float s"
        ")\n"
        "{\n"
        "    float a = dot(capsuleA, capsuleDirection);"
        "    float b = dot(rayDirection, capsuleDirection);\n"
        "    float c = dot(capsuleA, rayDirection);\n"
        "    float l = dot(capsuleA, capsuleA);\n"
        "    float r2 = radius*radius;\n"
        "    float determinant = (a*b-c)*(a*b-c)-(1.0-b*b)*(l-a*a-r2);\n"
        "    if (determinant < 0)\n"
        "    {\n"
        "        if (sphereHit(rayDirection, capsuleA, radius, viewPos, viewNormal)){ projectedHitPoint = capsuleA; s = 0.0; return true; }\n"
        "        if (sphereHit(rayDirection, capsuleB, radius, viewPos, viewNormal)){ projectedHitPoint = capsuleB; s = midLength; return true; }\n"
        "        return false;"
        "    \n}"
        "    float d = sqrt(determinant);\n"
        //   Ray hit point.
        "    float t = min( (-(a*b-c)+d)/(1.0-b*b), (-(a*b-c)-d)/(1.0-b*b) );"
        "    viewPos = rayDirection * t;\n"
        "    s = -dot(capsuleA-viewPos, capsuleDirection);\n"
        "    if (s < 0){\n"
        "        if (sphereHit(rayDirection, capsuleA, radius, viewPos, viewNormal)){ projectedHitPoint = capsuleA; s = 0.0; return true; }\n"
        "        return false;"
        "    \n}"
        "    if (s > midLength){\n"
        "        if (sphereHit(rayDirection, capsuleB, radius, viewPos, viewNormal)){ projectedHitPoint = capsuleB; s = midLength; return true; }\n"
        "        return false;"
        "    }"
        "    vec3 u = s*capsuleDirection;\n"
        "    projectedHitPoint = capsuleA+u;\n"
        "    viewNormal = normalize(viewPos-projectedHitPoint);\n"
        "    return true;\n"
        "}"
        "void main()\n"
        "{\n"
        "    vec3 lightViewPos = (view*lightPos).xyz;\n"
        "    vec3 rayDirection = normalize(vec3(billboard * bondScale, 0.0) + comViewPos);"
        "    float midLength = length(bViewPos-aViewPos);\n"
        "    vec3 capsuleDirection = normalize(bViewPos-aViewPos);\n"
        "    vec3 viewPos; vec3 viewNormal; vec3 projectedHitPoint; float s;\n"
        "    bool hit = capsuleHit(rayDirection, capsuleDirection, aViewPos, bViewPos, bondScale, midLength, viewPos, viewNormal, projectedHitPoint, s);\n"
        "    if (!hit) { discard; }\n"
        "    else\n"
        "    {\n"
        "       vec4 clipPos = proj * vec4(viewPos, 1.0);\n"
        "       float ndcDepth = clipPos.z / clipPos.w;\n"
        "       gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;\n"
        "       float diff = max(dot(viewNormal, normalize(lightViewPos-projectedHitPoint)), 0.0);\n"
        "       vec4 col = a_colour;\n"
        "       if (s > 0.5*midLength) { col = b_colour; }\n"
        "       colour = vec4((ambientLight + diff)*lightColour.rgb * col.rgb, col.a);\n"
        "    }\n"
        "}";

    void setProjectionView()
    {
        shader->setUniform<glm::mat4>("view", view);
        shader->setUniform<glm::mat4>("proj", projection);
    }

    void init()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &a_vertices);
        glGenBuffers(1, &b_vertices);
        glGenBuffers(1, &a_colours);
        glGenBuffers(1, &b_colours);
        glGenBuffers(1, &a_quad);

        positionsAAndScale.resize(4*maxBonds);
        positionsBAndScale.resize(4*maxBonds);
        coloursA.resize(4*maxBonds);
        coloursB.resize(4*maxBonds);

        glBindVertexArray(vao);

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

            createBuffer
            (
                a_vertices,
                positionsAAndScale.data(),
                positionsAAndScale.size(),
                GL_DYNAMIC_DRAW,
                1,
                4,
                1
            );

            createBuffer
            (
                b_vertices,
                positionsBAndScale.data(),
                positionsBAndScale.size(),
                GL_DYNAMIC_DRAW,
                2,
                4,
                1
            );

            createBuffer
            (
                a_colours,
                coloursA.data(),
                coloursA.size(),
                GL_DYNAMIC_DRAW,
                3,
                4,
                1
            );

            createBuffer
            (
                b_colours,
                coloursB.data(),
                coloursB.size(),
                GL_DYNAMIC_DRAW,
                4,
                4,
                1
            );
        glBindVertexArray(0);
    }

    /**
     * @brief Set the buffer position to the start.
     *
     */
    void flip() { index = 0; bonds = 0; }

    /**
     * @brief Insert (update) a Bonds data.
     *
     * @param bond the Bond data to insert.
     * @param atoms the Atom data the Bond data refers to.
     */
    void insert
    (
        const Bond & bond,
        const std::vector<Atom> & atoms
    )
    {
        const Atom & a = atoms[bond.atomIndexA];
        const Atom & b = atoms[bond.atomIndexB];

        positionsAAndScale[index] = a.position.x;
        positionsAAndScale[index+1] = a.position.y;
        positionsAAndScale[index+2] = a.position.z;
        positionsAAndScale[index+3] = a.scale;

        positionsBAndScale[index] = b.position.x;
        positionsBAndScale[index+1] = b.position.y;
        positionsBAndScale[index+2] = b.position.z;
        positionsBAndScale[index+3] = b.scale;

        coloursA[index] = a.colour.r;
        coloursA[index+1] = a.colour.g;
        coloursA[index+2] = a.colour.b;
        coloursA[index+3] = a.colour.a;

        coloursB[index] = b.colour.r;
        coloursB[index+1] = b.colour.g;
        coloursB[index+2] = b.colour.b;
        coloursB[index+3] = b.colour.a;

        index += 4;
        bonds++;
    }

    /**
     * @brief Upload Bond data to the GPU.
     *
     */
    void updateVertexArray()
    {
        glBindVertexArray(vao);

            subFullBuffer(a_vertices, positionsAAndScale.data(), positionsAAndScale.size());
            subFullBuffer(b_vertices, positionsBAndScale.data(), positionsBAndScale.size());
            subFullBuffer(a_colours, coloursA.data(), coloursA.size());
            subFullBuffer(b_colours, coloursB.data(), coloursB.size());

        glBindVertexArray(0);
    }
};
    GLuint vao, a_vertices, b_vertices, a_colours, b_colours, a_quad;
#endif /* BONDRENDERER_H */
