#ifndef GLPARTICLES
#define GLPARTICLES

#include <jGL/OpenGL/gl.h>
#include <jGL/particles.h>
#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/texture.h>

namespace jGL::GL
{
    class glParticles : public Particles 
    {

    public:

        glParticles(size_t sizeHint);

        glParticles(std::vector<TexturedParticle> p);

        void draw(glm::mat4 proj);

        void setTexture(std::shared_ptr<Texture> tex) { texture = tex; }

        void update(Particles::UpdateInfo info);

        ~glParticles()
        {
            glDeleteVertexArrays(1, &vao); 
            glDeleteBuffers(1, &a_position);
            glDeleteBuffers(1, &a_colour);
            glDeleteBuffers(1, &a_texCoord);
        }

    private:

        GLuint vao, a_position, a_colour, a_texCoord;

        std::vector<float> position;
        std::vector<float> colour;
        std::vector<float> texCoord;

        std::shared_ptr<Texture> texture = nullptr;

        void initGL();
        void flatten(Particles::UpdateInfo info);

        glShader shader;

        const char * vertexShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "layout(location=0) in vec4 a_position;\n"
            "layout(location=1) in vec4 a_colour;\n"
            "layout(location=2) in vec4 a_texCoord;\n"
            "uniform mat4 proj;\n"
            "uniform float scale;\n"
            "out float theta;\n"
            "out vec4 o_colour;\n"
            "void main(void){"
                "o_colour = a_colour;\n"
                "theta = a_position.z;\n"
                "gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"
                "gl_PointSize = a_position.w*scale;\n"
            "}";

        const char * fragmentShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "uniform sampler2D sampler;\n"
            "uniform int textureless;\n"
            "in vec4 o_colour;\n"
            "in float theta;\n"
            "layout(location=0) out vec4 colour;\n"
            "void main(void){\n"
                "vec2 coord = 2.0 * gl_PointCoord - 1.0;"
                "float ct = cos(theta); float st = sin(theta);\n"
                "mat2 rot = mat2(ct, -st, st, ct);\n"
                "vec2 pos = rot * coord;\n"
                "colour = o_colour * texture(sampler, (pos+1.0)*0.5);\n"
            "}";
    };
}

#endif /* GLPARTICLES */
