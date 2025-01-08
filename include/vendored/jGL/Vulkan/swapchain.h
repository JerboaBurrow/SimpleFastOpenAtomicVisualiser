#ifndef SWAPCHAIN
#define SWAPCHAIN

#include <jGL/Vulkan/Device/device.h>
#include <jLog/jLog.h>
#include <sstream>

#include <limits>
#include <algorithm>
#include <vector>

namespace jGL::Vulkan
{
    class Swapchain
    {

    public:

        Swapchain() = default;

        Swapchain(unsigned width, unsigned height, const Device & device, const VkSurfaceKHR & surface)
        {
            createSwapchain(width, height, device, surface);
        }

        void cleanupSwapchain(const Device & device);

        void recreateSwapchain(unsigned width, unsigned height, const Device & device, const VkSurfaceKHR & surface);

        const VkFormat & getImageFormat() const { return swapchainImageFormat; }
        const VkExtent2D & getVkExtend2D() const { return swapchainExtent; }

        const std::vector<VkImageView> & getVkImageViews() const { return swapchainImageViews; }

        const VkSwapchainKHR & getVkSwapchainKHR() { return swapchain; }

    private:

        VkSwapchainKHR swapchain;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;

        std::vector<VkImage> swapchainImages;

        std::vector<VkImageView> swapchainImageViews;

        void createSwapchain(unsigned width, unsigned height, const Device & device, const VkSurfaceKHR & surface);

        VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
        VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
        VkExtent2D chooseSwapExtent(unsigned width, unsigned height, const VkSurfaceCapabilitiesKHR & capabilities);
        void createImageViews(const Device & device);

    };
}

#endif /* SWAPCHAIN */
