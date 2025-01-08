#ifndef GLSHADER
#define GLSHADER

#include <jGL/OpenGL/gl.h>
#include <jGL/shader.h>

/*

    Create a shader like so
    
        Shader s(vs, fs) # for std::strings vs and fs
    
    Uniforms are auto scraped in the form "uniform TYPE NAME;"
    
    Get and Set like so
    
        s.getUniform<int>("anInteger");
        s.setUniform<glm::mat4>("projection",myMat4Variable);
    
    Use, compile and release

        s.compile()     # compiles code, creates a program if its not one
        s.use();        # glUseProgram
        s.release();    # glDeleteProgram 
        
*/

namespace jGL::GL
{

    struct glShader : public Shader
    {

        glShader(const char * v, const char * f)
        : Shader(v, f), program(0), compiled(false), used(false)
        {}

        glShader()
        : Shader(), program(0), compiled(false), used(false)
        {}

        glShader(std::string path, std::string name)
        : Shader(path, name), program(0), compiled(false), used(false)
        {}

        ~glShader(){if(isProgram()){release();}}

        void create();
        void release();
        void compile();
        void use();
        bool isCompiled(){return compiled;}
        bool isProgram(){return glIsProgram(program);}

    private:

        GLuint program;
        bool compiled;
        bool used;

        inline const GLuint location(const char * name) const
        {
            return glGetUniformLocation(program, name);
        }

        // cannot have spec in class scope https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
        //  also cannot use partial spec workaround because non-class, non-variable partial 
        //  specialization is not allowed
        //  https://stackoverflow.com/questions/8061456/c-function-template-partial-specialization
        // so the template dispatches to these guys

        void setValue(jGLUniform<int> * u, int value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniform1i(location(u->name.c_str()), u->value);
            }

        }

        void setValue(jGLUniform<Sampler2D> * u, Sampler2D value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniform1i(location(u->name.c_str()), u->value.texture);
            }

        }

        void setValue(jGLUniform<float> * u, float value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniform1f(location(u->name.c_str()), u->value);
            }

        }

        void setValue(jGLUniform<glm::vec2> * u, glm::vec2 value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniform2f(location(u->name.c_str()), u->value.x, u->value.y);
            }

        }

        void setValue(jGLUniform<glm::vec4> * u, glm::vec4 value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniform4f(location(u->name.c_str()), u->value.x, u->value.y, u->value.z, u->value.w);
            }

        }

        void setValue(jGLUniform<glm::mat4> * u, glm::mat4 value)
        {
            u->value = value;
            if (isCompiled())
            {
                use();
                glUniformMatrix4fv(location(u->name.c_str()), 1, false, glm::value_ptr(u->value));
            }
        }

    };

}
#endif /* GLSHADER */
