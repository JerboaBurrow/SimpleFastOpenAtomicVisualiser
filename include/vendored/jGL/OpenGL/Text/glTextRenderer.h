#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <jGL/OpenGL/gl.h>
#include <jGL/OpenGL/Text/glFont.h>
#include <jGL/OpenGL/Shader/glShader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL::GL
{

  class TextRenderer 
  {

  public:

    TextRenderer(glm::vec2 res);

    ~TextRenderer()
    {
      glDeleteBuffers(1,&VBO);
      glDeleteVertexArrays(1,&VAO);
    }

    void renderText
    (
      glFont font,
      std::string text,
      glm::vec2 position,
      float scale,
      glm::vec4 colour,
      glm::vec2 res,
      glm::bvec2 centre = glm::bvec2(false,false)
    );

    void setProjection(glm::mat4 p)
    {
      
      projection = p;

      shader.use();
      shader.setUniform<glm::mat4>("proj", projection);

    }

    void clear()
    {
      vertices.clear();
      charactersUploaded = 0;
      setBufferSize(512);
    }

  private:
  
    const char * vert = "#version " GLSL_VERSION "\n"
      "precision lowp float;\n precision lowp int;\n"
      "layout(location=0) in vec4 posTex;\n"
      "uniform mat4 proj;\n"
      "out vec2 texCoords;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = proj * vec4(posTex.xy, 0.0, 1.0);\n"
      "    texCoords = posTex.zw;\n"
      "}\n";

    const char * frag = "#version " GLSL_VERSION "\n"
      "precision lowp float;\n precision lowp int;\n"
      "in vec2 texCoords;\n"
      "layout(location=0) out vec4 colour;\n"
      "uniform vec4 textColour;\n"
      "uniform sampler2D glyph;\n"
      "void main()\n"
      "{\n"
      "   vec4 glpyhSample = vec4(1.0,1.0,1.0, texture(glyph,texCoords.xy).r);\n"
      "   colour = textColour*glpyhSample;\n"
      "}";

    glShader shader;
    GLuint VAO;
    GLuint VBO;

    uint16_t charactersUploaded;
    std::vector<float> vertices;

    glm::mat4 projection;
    glm::vec2 res;


    void setBufferSize(uint16_t s);

  };
}
#endif /* TEXTRENDERER_H */
