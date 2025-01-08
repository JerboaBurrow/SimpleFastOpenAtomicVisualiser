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


    struct Shader 
    {

        /*
        
            A generic Shader object that can read vertex and fragment shaders from 
                file or from const char *'s

                Uniforms are detected within the shader program
        
        */

        Shader(const char * v, const char * f)
        : vertex(v), fragment(f)
        {
            parseUniforms();
        }

        Shader()
        : vertex(""),fragment("")
        {}

        Shader(std::string path, std::string name);

        virtual ~Shader() = default;

        bool operator==(const Shader & s)
        {
            return this->vertex == s.vertex && this->fragment == s.fragment;
        }

        bool lint();
        
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

        std::vector<std::string> getUniformNames()
        {
            std::vector<std::string> v;
            for (auto it = uniforms.cbegin(); it != uniforms.cend(); it++)
            {
                v.push_back(it->first);
            }
            return v;
        }

        virtual void use() = 0;

    protected:

        std::string vertex;
        std::string fragment;

        std::unordered_map<std::string, std::shared_ptr<AbstractjGLUniform>> uniforms;

        std::string parseShaderSource(std::ifstream & file);

        virtual void compile() = 0;

        bool parseUniforms();
        
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
