#ifndef FRAMEBUFFER
#define FRAMEBUFFER

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/swapchain.h>
#include <jGL/Vulkan/Texture/vkTexture.h>
#include <jGL/Vulkan/RenderPass/renderPass.h>

namespace jGL::Vulkan
{
    class Framebuffer
    {

        /*
        
            Simple framebuffer supporting one colour attachment with MSAA
                refer also to jGL::Vulkan::RenderPass
        
        */
    
    public:

        Framebuffer
        (
            const Device & device, 
            const Swapchain & swapchain, 
            const RenderPass & renderPass,
            const VkImageView & msaaView,
            const VkImageView & resolveView
        );

        ~Framebuffer()
        {
            vkDestroyFramebuffer(device.getVkDevice(), buffer, nullptr);
        }

        const VkFramebuffer & getVkFramebuffer() const { return buffer; }

    private:

        const Device & device;
        VkFramebuffer buffer;
    };
}

#endif /* FRAMEBUFFER */
