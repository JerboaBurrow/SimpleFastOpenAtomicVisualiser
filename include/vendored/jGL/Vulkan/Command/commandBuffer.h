#ifndef COMMANDBUFFER
#define COMMANDBUFFER

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/Command/commandPool.h>

#include <vector>

namespace jGL::Vulkan
{
    class CommandBuffer
    {

    public:

        CommandBuffer() = default;

        CommandBuffer(const Device & device,  const CommandPool & commandPool, const unsigned concurrentFrames);

        const std::vector<VkCommandBuffer> & getVkCommandBuffers() const { return commandBuffers; }

    private:

        std::vector<VkCommandBuffer> commandBuffers;

    };
} 

#endif /* COMMANDBUFFER */
