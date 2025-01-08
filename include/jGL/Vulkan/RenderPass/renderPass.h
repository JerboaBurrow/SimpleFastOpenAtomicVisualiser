#ifndef RENDERPASS
#define RENDERPASS

#include <vulkan/vulkan.h>
#include <array>
#include <jGL/Vulkan/Device/device.h>

namespace jGL::Vulkan
{
    class RenderPass
    {

    /*
    
        A bare bones render pass, supporting MSAA on a single 
            colour attachement
    
    */
    
    public:

        RenderPass() = default;

        RenderPass
        (
            const Device & device, 
            VkSampleCountFlagBits msaaSamples,
            VkFormat swapChainImageFormat
        );

        const VkRenderPass & getVkRenderPass() const { return renderPass; }

    private:

        VkRenderPass renderPass;

    };
}

#endif /* RENDERPASS */
