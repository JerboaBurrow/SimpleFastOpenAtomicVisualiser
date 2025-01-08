#ifndef PIPELINE
#define PIPELINE

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/RenderPass/renderPass.h>
#include <jGL/Vulkan/Shader/vkShader.h>
#include <jGL/Vulkan/Shader/vertexBuffer.h>
#include <jGL/Vulkan/Shader/uniformBuffer.h>
#include <jGL/Vulkan/Descriptor/descriptorSets.h>
#include <jGL/Vulkan/Texture/vkSampler.h>
#include <jGL/Vulkan/Texture/vkTexture.h>

#include <memory>

namespace jGL::Vulkan
{
    class Pipeline
    {

        /*
        
            Single "shader" pipeline (vertex, fragment, ++) supporting msaa, dynamic
                viewport and scissor. As well as given topology, polygonMode, blending 
                and descriptor set layouts at instance

        */
    
    public:

        Pipeline
        (
            const Device & device, 
            const RenderPass & renderPass,
            const vkShader & shader,
            std::vector<std::shared_ptr<VertexBufferObject>> vertexInputs,
            std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
            std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
            const uint32_t concurrentFrames,
            const VkPrimitiveTopology & topology,
            const VkViewport & viewport,
            const VkRect2D & scissor,
            const VkPolygonMode & polygonMode,
            const VkSampleCountFlagBits & msaaSamples,
            const VkPipelineColorBlendStateCreateInfo & blending
        );

        const VkPipeline & getVkPipeline() const { return pipeline; }
        const VkPipelineLayout & getVkPipelineLayout() const { return pipelineLayout; }

        const std::vector<VkDescriptorSet> getVkDescriptorSets(uint32_t currentFrame);

        ~Pipeline()
        {
            vkDestroyDescriptorSetLayout(device.getVkDevice(), uniformDescriptorSetLayout, nullptr);
            vkDestroyDescriptorSetLayout(device.getVkDevice(), imageDescriptorSetLayout, nullptr);
            vkDestroyPipelineLayout(device.getVkDevice(), pipelineLayout, nullptr);
            vkDestroyPipeline(device.getVkDevice(), pipeline, nullptr);
        }

    private:

        const Device & device;

        DescriptorPool uniformDescriptorPool, imageDescriptorPool;
        VkDescriptorSetLayout uniformDescriptorSetLayout, imageDescriptorSetLayout;
        DescriptorSets<VkDescriptorBufferInfo> uniformDescriptors;
        DescriptorSets<VkDescriptorImageInfo> imageDescriptors;

        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

        void createDescriptorSetLayouts
        (
            std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
            std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
            const uint32_t concurrentFrames
        );

        void createDescriptorSets
        (
            std::vector<std::shared_ptr<UniformBufferObject>> uniforms,
            std::vector<std::pair<VkImageView, std::shared_ptr<Sampler> >> & textures,
            const uint32_t concurrentFrames
        );
    };
}

#endif /* PIPELINE */
