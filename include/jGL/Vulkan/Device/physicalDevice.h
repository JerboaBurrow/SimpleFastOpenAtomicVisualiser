#ifndef PHYSICALDEVICE
#define PHYSICALDEVICE

#include <vulkan/vulkan.h>
#include <exception>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <optional>
#include <set>

#include <jLog/jLog.h>

namespace jGL::Vulkan
{

    const std::vector<const char *> deviceExtensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class PhysicalDevice
    {
    
    public:

        PhysicalDevice() = default;

        PhysicalDevice(const VkInstance & instance, const VkSurfaceKHR & surface);

        const VkPhysicalDevice & getVkPhysicalDevice() const { return physicalDevice; }
        const QueueFamilyIndices & getQueueFamilyIndices() const { return queueFamilyIndices; }
        const SwapchainSupportDetails & getSwapchainSupportDetails() const { return swapchainSupport; }
        const VkPhysicalDeviceLimits & getLimits() const { return deviceLimits; }

        const VkSampleCountFlagBits & getMaxMsaa() const { return msaaSamples; }

    private:

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        std::vector<std::pair<std::string, VkPhysicalDevice>> alternatives;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPhysicalDeviceLimits deviceLimits;

        QueueFamilyIndices queueFamilyIndices;
        SwapchainSupportDetails swapchainSupport;

        void pick(const VkInstance & instance, const VkSurfaceKHR & surface);
        
        bool isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface) const;
        QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) const;
        bool checkDeviceExtensionSupport(const VkPhysicalDevice physicalDevice) const;
        SwapchainSupportDetails querySwapchainSupport(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) const;
        VkSampleCountFlagBits getMaxUsableSampleCount() const;
    };
}

#endif /* PHYSICALDEVICE */
