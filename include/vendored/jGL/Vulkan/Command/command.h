#ifndef COMMAND
#define COMMAND

#include <jGL/Vulkan/Command/commandBuffer.h>
#include <jGL/Vulkan/Command/commandPool.h>

#include <jGL/Vulkan/Device/device.h>

namespace jGL::Vulkan
{
    class Command
    {

    public:

        Command() = default;

        Command(const Device & device, const unsigned concurrentFrames)
        : commandPool(CommandPool(device))
        {
            commandBuffer = CommandBuffer(device, commandPool, concurrentFrames);
        }

        const VkCommandPool & getVkCommandPool() const { return commandPool.getVkCommandPool(); }
        const std::vector<VkCommandBuffer> & getVkCommandBuffers() const { return commandBuffer.getVkCommandBuffers(); }
        const VkCommandBuffer & getVkCommandBuffer(size_t i) const { return commandBuffer.getVkCommandBuffers()[i]; }

        VkCommandBuffer beginCommand(const Device & device)  const
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = commandPool.getVkCommandPool();
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            return commandBuffer;
        }

        void endCommand(const Device & device, VkCommandBuffer commandBuffer) const {
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(device.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(device.getLogicalDevice().getGraphicsQueue());

            vkFreeCommandBuffers(device.getVkDevice(), commandPool.getVkCommandPool(), 1, &commandBuffer);
        }

    private:

        CommandPool commandPool;
        CommandBuffer commandBuffer;
    };
}

#endif /* COMMAND */
