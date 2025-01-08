#ifndef VKTEXTRENDERER_H
#define VKTEXTRENDERER_H

#include <jGL/Vulkan/pipeline.h>
#include <jGL/Vulkan/Text/vkFont.h>
#include <jGL/Vulkan/Command/command.h>
#include <memory>

namespace jGL::Vulkan
{

  class TextRenderer 
  {

  public:

    TextRenderer
    (
      const Device & device,
      const Command & command,
      const RenderPass & renderPass,
      const VkViewport & viewport,
      const VkRect2D & scissor,
      uint32_t concurrentFrames,
      VkSampleCountFlagBits msaa
    );

    ~TextRenderer(){}

    void renderText
    (
      const Device & device,
      const Command & command,
      const VkCommandBuffer & commandBuffer,
      uint32_t currentFrame,
      std::string text,
      glm::vec2 position,
      float scale,
      glm::vec4 colour,
      glm::bvec2 centre = glm::bvec2(false,false)
    );

    void setProjection(glm::mat4 p);

  private:

    const char * vert = "#version 450\n"
      "layout(location = 0) in vec4 posTex;\n"
      "layout(set = 0, binding = 0) uniform vUBO { mat4 proj; } ubo;\n"
      "layout(location = 0) out vec2 texCoords;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = ubo.proj * vec4(posTex.xy, 0.0, 1.0);\n"
      "    texCoords = posTex.zw;\n"
      "}\n";

    const char * frag = "#version 450\n"
      "layout(location = 0) in vec2 texCoords;\n"
      "layout(location = 0) out vec4 colour;\n"
      "layout(set = 0, binding = 1) uniform fUBO { vec4 textColour; } ubo;\n"
      "layout(set = 1, binding = 0) uniform usampler2D glyph;\n"
      "void main()\n"
      "{\n"
      "   vec4 glpyhSample = vec4(1.0,1.0,1.0, texture(glyph,texCoords.xy).r);\n"
      "   colour = ubo.textColour*glpyhSample;\n"
      "}";

    vkShader shader;

    std::shared_ptr<VertexBufferObject> pos;
    std::shared_ptr<UniformBufferObject> uboV, uboF;
    std::vector<glm::vec4> vertices;

    VkImageView characterTextureView;

    std::shared_ptr<vkTexture> fontTexture;
    std::shared_ptr<Sampler> fontSampler;

    std::unique_ptr<vkFont> font;

    std::unique_ptr<Pipeline> textPipeline;

    struct vUBO {glm::mat4 proj;};
    struct fUBO {glm::vec4 colour;};

    glm::ivec2 res;

  };
}

#endif /* TEXTRENDERER */
