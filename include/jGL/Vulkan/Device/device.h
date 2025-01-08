#ifndef DEVICE
#define DEVICE

#include <jGL/Vulkan/Device/logicalDevice.h>
#include <jGL/Vulkan/Device/physicalDevice.h>

namespace jGL::Vulkan
{
    class Device
    {
    
    public:

        Device() = default;
    
        Device(PhysicalDevice pd, LogicalDevice ld)
        : physicalDevice(pd), logicalDevice(ld)
        {}

        const PhysicalDevice & getPhysicalDevice() const { return physicalDevice; }
        const VkPhysicalDevice & getVkPhysicalDevice() const { return physicalDevice.getVkPhysicalDevice(); }

        const LogicalDevice & getLogicalDevice() const { return logicalDevice; }
        const VkDevice & getVkDevice() const { return logicalDevice.getVkDevice(); }
        const VkPhysicalDeviceLimits & getLimits() const { return physicalDevice.getLimits(); }

    private:

        PhysicalDevice physicalDevice;
        LogicalDevice logicalDevice;

    };
}

#endif /* DEVICE */
