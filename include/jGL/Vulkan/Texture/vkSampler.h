#ifndef VKSAMPLER
#define VKSAMPLER

#include <jGL/Vulkan/Device/device.h>

namespace jGL::Vulkan
{
    class Sampler
    {
    
    public:

        Sampler
        (
            const Device & device,
            VkFilter filter = VK_FILTER_LINEAR,
            uint32_t binding = 0
        )
        : device(device), binding(binding)
        {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = filter;
            samplerInfo.minFilter = filter;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = device.getLimits().maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            // texels are [0,1]
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            // can have a texel comparison on sampling
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 0.0f;

            if (vkCreateSampler(device.getVkDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create texture sampler");
            }
        }

        ~Sampler()
        {
            vkDestroySampler(device.getVkDevice(), sampler, nullptr);
        }

        const VkSampler & getVkSampler() const { return sampler; }

        void setBinding(uint32_t b) { binding = b; }

        VkDescriptorSetLayoutBinding getLayout() const
        {
            VkDescriptorSetLayoutBinding layout {};
            layout.binding = binding;
            layout.descriptorCount = 1;
            layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            layout.pImmutableSamplers = nullptr;
            layout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            return layout;
        }

    private:

        const Device & device;

        VkSampler sampler;

        uint32_t binding;

    };
}

#endif /* VKSAMPLER */
