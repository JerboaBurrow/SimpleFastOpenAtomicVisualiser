#ifndef GLSHADER
#define GLSHADER

#include <jGL/OpenGL/gl.h>
#include <jGL/shader.h>

namespace jGL::GL
{

    /**
     * @brief An OpenGL implementation of Shader.
     * 
     */
    struct glShader : public Shader
    {

        /**
         * @brief Construct a glShader from a vertex and fragment source.
         * 
         * @param v vertex shader source.
         * @param f fragment shader source.
         */
        glShader(const char * v, const char * f)
        : Shader(v, f), program(0), compiled(false), used(false)
        {}

        /**
         * @brief Construct an empty glShader.
         */
        glShader()
        : Shader(), program(0), compiled(false), used(false)
        {}

        /**
         * @brief Construct a glShader from source given by paths.
         * 
         * @param path path the .vs and .fs shader sources.
         * @param name name of sources in path: name.vs and name.fs
         */
        glShader(std::string path, std::string name)
        : Shader(path, name), program(0), compiled(false), used(false)
        {}

        ~glShader(){if(isProgram()){release();}}

        /**
         * @brief Create the shader program.
         * 
         */
        void create();

        /**
         * @brief Destroy the shader program.
         * 
         */
        void release();

        /**
         * @brief Compile the shader program.
         * @remark If not a program, call glShader::create.
         */
        void compile();

        /**
         * @brief Use the shader program (and compile if required).
         * @remark If not compiled call glShader::compile.
         */
        void use();

        /**
         * @brief Checks if the glShader is compiled.
         * 
         * @return true it is compiled.
         * @return false it is not compiled.
         */
        bool isCompiled(){return compiled;}

        /**
         * @brief Checks if an OpenGL program is created.
         * 
         * @return true a program has been created.
         * @return false a program has not been created.
         */
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
