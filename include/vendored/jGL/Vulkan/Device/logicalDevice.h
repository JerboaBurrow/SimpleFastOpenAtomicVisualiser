#ifndef LOGICALDEVICE
#define LOGICALDEVICE

#include <jGL/Vulkan/vkDebug.h>
#include <jGL/Vulkan/Device/physicalDevice.h>

namespace jGL::Vulkan
{
    class LogicalDevice
    {
    
    public:
    
        LogicalDevice() = default;
        
        LogicalDevice(const PhysicalDevice & physicalDevice);

        const VkDevice & getVkDevice() const { return logicalDevice; }
        // might encapsulate queue elsewhere?
        const VkQueue & getGraphicsQueue() const { return graphicsQueue; }
        const VkQueue & getPresentQueue() const { return presentQueue; }

    private:

        VkDevice logicalDevice;

        VkQueue graphicsQueue, presentQueue;

    };
}

#endif /* LOGICALDEVICE */
