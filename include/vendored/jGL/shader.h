#ifndef SHADER
#define SHADER

#include <jGL/uniform.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <regex>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <memory>
#include <jLog/jLog.h>
#include <typeinfo>

namespace jGL
{

    template <class T>
    const std::regex UNIFORM_DATA_REGEX;

    template <> inline
    const std::regex UNIFORM_DATA_REGEX<int> = std::regex("uniform int (\\S+);");
    template <> inline
    const std::regex UNIFORM_DATA_REGEX<float> = std::regex("uniform float (\\S+);");
    template <> inline
    const std::regex UNIFORM_DATA_REGEX<glm::vec2> = std::regex("uniform vec2 (\\S+);");
    template <> inline
    const std::regex UNIFORM_DATA_REGEX<glm::vec4> = std::regex("uniform vec4 (\\S+);");
    template <> inline
    const std::regex UNIFORM_DATA_REGEX<glm::mat4> = std::regex("uniform mat4 (\\S+);");
    template <> inline
    const std::regex UNIFORM_DATA_REGEX<Sampler2D> = std::regex("uniform(\\slowp\\s|\\shighp\\s|\\smediump\\s|\\s)sampler2D (\\S+);");


    /**
     * @brief Generic shader program with vertex and fragment shader.
     * @remark Uniforms are automatically detected.
     * 
     */
    struct Shader 
    {
        /**
         * @brief Construct a Shader from a vertex and fragment source.
         * 
         * @param v vertex shader source.
         * @param f fragment shader source.
         */
        Shader(const char * v, const char * f)
        : vertex(v), fragment(f)
        {
            parseUniforms();
        }

        /**
         * @brief Construct an empty Shader.
         */
        Shader()
        : vertex(""),fragment("")
        {}

        /**
         * @brief Construct a Shader from source given by paths.
         * 
         * @param path path the .vs and .fs shader sources.
         * @param name name of sources in path: name.vs and name.fs
         */
        Shader(std::string path, std::string name);

        virtual ~Shader() = default;

        bool operator==(const Shader & s)
        {
            return this->vertex == s.vertex && this->fragment == s.fragment;
        }

        /**
         * @brief Check for common shader errors.
         * 
         * @return true linting passes.
         * @return false linting fails.
         */
        bool lint();
        
        /**
         * @brief Set a Uniform to a value.
         * 
         * @tparam T the type of uniform.
         * @param name the name of the uniform.
         * @param value the value of type T.
         */
        template <class T>
        void setUniform(std::string name, T value)
        { 
            if (uniforms.find(name) == uniforms.end())
            {
                throw std::runtime_error("could not find uniform: " + name);
            }

            AbstractjGLUniform * uniform = uniforms[name].get();

            if (dynamic_cast<jGLUniform<T>*>(uniform) == nullptr)
            {
                return;
            }

            jGLUniform<T> * u = dynamic_cast<jGLUniform<T>*>(uniform);
            if (u != nullptr){ setValue(u, value); return; }

            return;
        }

        const std::string & getVertex() const { return vertex; }
        const std::string & getFragment() const { return fragment; }

        /**
         * @brief Get a Uniform by name.
         * 
         * @tparam T the type of the uniform.
         * @param name the name of the uniform to get.
         * @return jGLUniform<T> the uniform found.
         */
        template <class T>
        jGLUniform<T> getUniform(std::string name)
        {
            if (uniforms.find(name) == uniforms.end())
            {
                return NULL_UNIFORM<T>;
            }

            AbstractjGLUniform * uniform = uniforms[name].get();

            if (dynamic_cast<jGLUniform<T>*>(uniform) == nullptr)
            {
                return NULL_UNIFORM<T>;
            }

            jGLUniform<T> * u = dynamic_cast<jGLUniform<T>*>(uniform);
            if (u != nullptr){ return *u; }

            return NULL_UNIFORM<T>;
        }

        /**
         * @brief Get all the parsed uniforms in the Shader.
         * 
         * @return std::vector<std::string> the available uniform names.
         */
        std::vector<std::string> getUniformNames()
        {
            std::vector<std::string> v;
            for (auto it = uniforms.cbegin(); it != uniforms.cend(); it++)
            {
                v.push_back(it->first);
            }
            return v;
        }

        /**
         * @brief Use the shader.
         * @remark Use will auto-compile if required.
         */
        virtual void use() = 0;

        /**
         * @brief Display the vertex shader with line numbers.
         * 
         * @return std::string the formatted vertex shader
         */
        std::string displayVertexSource() const { return formatWithLineNumbers(vertex); }
        
        /**
         * @brief Display the fragment shader with line numbers.
         * 
         * @return std::string the formatted fragment shader
         */
        std::string displayFragmentSource() const { return formatWithLineNumbers(fragment); }

    protected:

        std::string vertex;
        std::string fragment;

        std::unordered_map<std::string, std::shared_ptr<AbstractjGLUniform>> uniforms;

        std::string parseShaderSource(std::ifstream & file);

        virtual void compile() = 0;

        bool parseUniforms();

        std::string formatWithLineNumbers(std::string shader) const
        {
            if (shader.length() == 0) { return shader; }
            std::string source = "1: ";
            uint32_t line = 1;
            auto iter = shader.begin();
            while (iter != shader.end())
            {
                if (*iter == '\n' && iter+1 != shader.end())
                {
                    line += 1;
                    source += "\n" + std::to_string(line)+": ";
                }
                else
                {
                    source += *iter;
                }
                iter++;
            }
            return source;
        }
        
        template <class T>
        void detectUniformsAndCreate(std::string code)
        {
            const std::regex r = UNIFORM_DATA_REGEX<T>;

            auto start = std::sregex_iterator(code.begin(), code.end(), r);
            auto end = std::sregex_iterator();
            for (std::sregex_iterator it = start; it != end; it++)
            {
                std::string match = (*it).str();
                if (match.find("lowp") != std::string::npos)
                {
                    // uniform lowp TYPE NAME;
                    match.erase(0, match.find("lowp")+4);
                    // " TYPE NAME;"
                }
                if (match.find("highp") != std::string::npos)
                {
                    // uniform highp TYPE NAME;
                    match.erase(0, match.find("highp")+5);
                    // " TYPE NAME;"
                }
                // found "uniform TYPE NAME;" or " TYPE NAME;"
                match.erase(0, match.find(" ")+1);
                match.erase(0, match.find(" ")+1);
                std::string name = match.substr(0, match.find(";"));
                std::shared_ptr<jGLUniform<T>> u = std::make_shared<jGLUniform<T>>(jGLUniform<T>(name,T(0)));
                uniforms[name] = u;
            }

        }

        // cannot have spec in class scope https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
        //  also cannot use partial spec workaround because non-class, non-variable partial 
        //  specialization is not allowed
        //  https://stackoverflow.com/questions/8061456/c-function-template-partial-specialization
        // so the template dispatches to these guys

        virtual void setValue(jGLUniform<int> * u, int value)
        {
            u->value = value;

        }

        virtual void setValue(jGLUniform<Sampler2D> * u, Sampler2D value)
        {
            u->value = value;
        }

        virtual void setValue(jGLUniform<float> * u, float value)
        {
            u->value = value;
        }

        virtual void setValue(jGLUniform<glm::vec2> * u, glm::vec2 value)
        {
            u->value = value;
        }

        virtual void setValue(jGLUniform<glm::vec4> * u, glm::vec4 value)
        {
            u->value = value;
        }

        virtual void setValue(jGLUniform<glm::mat4> * u, glm::mat4 value)
        {
            u->value = value;
        }
    };
}

#endif /* SHADER */
