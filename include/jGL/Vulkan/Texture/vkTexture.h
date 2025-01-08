#ifndef VKTEXTURE_H
#define VKTEXTURE_H

#include <jGL/texture.h>
#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/buffer.h>

#include <glm/glm.hpp>

#include <stdexcept>
#include <filesystem>

namespace jGL::Vulkan
{

    void createImage
    (        
        const Device & device,
        uint32_t width, 
        uint32_t height,
        uint32_t mipLevels, 
        VkSampleCountFlagBits numSamples,
        VkFormat format, 
        VkImageTiling tiling, 
        VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties,
        VkImage & image,
        VkDeviceMemory & memory
    );

    void createImageView
    (
        const Device & device,
        VkFormat format, 
        VkImageAspectFlags aspectFlags, 
        uint32_t mipLevels,
        VkImage & image,
        VkImageView & imageView
    );

    class vkTexture : public Texture 
    {
        
    public:

        vkTexture(const Device & device);
        
        vkTexture
        (
            const Device & device,
            const Command & command,
            std::filesystem::path imageFile, 
            Texture::Type type,
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT
        );

        vkTexture 
        (
            const Device & device,
            const Command & command,
            uint32_t width, 
            uint32_t height, 
            uint32_t channels, 
            VkFormat format,
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT
        );

        vkTexture 
        (
            const Device & device,
            const Command & command,
            uint32_t width, 
            uint32_t height, 
            uint32_t channels, 
            VkFormat format, 
            std::vector<std::byte> pixels,
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT
        );

        const VkImageView & getVkImageView() const { return imageView; }

        ~vkTexture()
        {
            vkDestroyImageView(device.getVkDevice(), imageView, nullptr);
            vkDestroyImage(device.getVkDevice(), image, nullptr);
            vkFreeMemory(device.getVkDevice(), imageMemory, nullptr);
        }

        void bind(unsigned unit) {/*TODO*/}

        void replace
        (
            const Command & command,
            uint32_t width, 
            uint32_t height, 
            uint32_t channels, 
            VkFormat format,
            std::vector<std::byte> pixels
        );

    protected:

        const Device & device;

        VkImage image;
        VkImageView imageView;
        VkDeviceMemory imageMemory;

        VkFormat format;
        uint32_t width, height, channels;
        VkSampleCountFlagBits msaaSamples;

        void transitionImageLayout
        (
            const Command & command,
            VkFormat format, 
            VkImageLayout oldLayout, 
            VkImageLayout newLayout
        );

        void copyBufferToImage
        (
            const Command & command,
            Buffer & buffer, 
            uint32_t width, 
            uint32_t height
        );

        void uploadImage
        (
            const Command & command,
            std::vector<std::byte> pixels,
            VkFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t channels
        );

    };

}

#endif /* VKTEXTURE_H */
