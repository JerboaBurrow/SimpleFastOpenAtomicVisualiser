#ifndef COMMANDPOOL
#define COMMANDPOOL

#include <jGL/Vulkan/Device/device.h>

namespace jGL::Vulkan
{
    class CommandPool
    {

    public:

        CommandPool() = default;

        CommandPool(const Device & device);

        const VkCommandPool & getVkCommandPool() const { return commandPool; }

    private:

        VkCommandPool commandPool;

    };
}

#endif /* COMMANDPOOL */
