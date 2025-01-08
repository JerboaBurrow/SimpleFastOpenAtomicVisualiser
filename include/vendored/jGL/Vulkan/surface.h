#ifndef SURFACE
#define SURFACE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <exception>

namespace jGL::Vulkan
{
    class Surface
    {
    
    public:

        Surface() = default;

        Surface(const VkInstance & instance, GLFWwindow * window)
        {
            if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create window surface");
            }
        }

        const VkSurfaceKHR & getVkSurfaceKHR(){ return surface; }

    private:

        VkSurfaceKHR surface;
    };
}

#endif /* SURFACE */
