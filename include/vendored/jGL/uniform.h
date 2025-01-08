#ifndef UNIFORM
#define UNIFORM

#include <cmath>
#include <string>

namespace jGL
{
    struct AbstractjGLUniform
    {
        AbstractjGLUniform(std::string n)
        : name(n)
        {}

        virtual ~AbstractjGLUniform() = default;

        std::string name;
    };

    template <class T>
    struct jGLUniform : public AbstractjGLUniform
    {
        jGLUniform(std::string n, T v)
        : AbstractjGLUniform(n), value(v)
        {}

        T value;
    };

    template <class T>
    const jGLUniform NULL_UNIFORM("", static_cast<T>(NAN));

    struct Sampler2D { 

        Sampler2D(int t)
        : texture(t) 
        {}

        Sampler2D(unsigned t)
        : texture(t) 
        {}

        Sampler2D(float t)
        : texture(int(t)) 
        {}

        int texture; 
    };
}

#endif /* UNIFORM */
