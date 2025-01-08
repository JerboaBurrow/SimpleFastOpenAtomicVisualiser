#ifndef GL_H
#define GL_H

#ifdef ANDROID

/*

https://developer.android.com/ndk/guides/stable_apis

    OpenGL ES 1.0 - 3.2

    The standard OpenGL ES 1.x headers (<GLES/gl.h> and <GLES/glext.h>), 
    2.0 headers (<GLES2/gl2.h> and <GLES2/gl2ext.h>), 
    3.0 headers (<GLES3/gl3.h> and <GLES3/gl3ext.h>), 
    3.1 headers (<GLES3/gl31.h> and <GLES3/gl3ext.h>), and 
    3.2 headers (<GLES3/gl32.h> and <GLES3/gl3ext.h>) contain the declarations necessary for OpenGL ES.

*/

#include <GLES3/gl31.h>

#else

#ifdef _WIN32
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#endif 

#include <GL/glew.h>

#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <string>

namespace jGL::GL
{

    class GLRuntimeException;
    // print buffer status errors
    GLuint glBufferStatus(const std::string msg = "");
    // print gl error codes
    GLuint glError(const std::string msg = "");

    // compile a gl shader given a program and source code as const char *
    void compileShader(GLuint & shaderProgram, const char * vert, const char * frag);

}
#endif