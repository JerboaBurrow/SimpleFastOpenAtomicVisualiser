#ifndef BUFFER
#define BUFFER

#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/Device/device.h>

#include <cstring>

namespace jGL::Vulkan
{
    class Buffer
    {

    public:

        Buffer
        (
            const Device & d,
            VkDeviceSize s,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        )
        : device(d), size(s)
        {
            createBuffer(size, usage, properties);
        }

        ~Buffer()
        {   
            vkDestroyBuffer(device.getVkDevice(), buffer, nullptr);
            vkFreeMemory(device.getVkDevice(), memory, nullptr);
        }

        void copy(const Buffer & src, const Command & command);

        const VkDeviceSize & getVkDeviceSize() const { return size; }

        void copyMemory
        (
            const VkDeviceSize offset,
            const VkDeviceSize size,
            void * data,
            const VkMemoryMapFlags = 0
        );

        const VkBuffer & getVkBuffer() const { return buffer; }
        const VkDeviceMemory & getVkDeviceMemory() const { return memory; }

    private:

        const Device & device;

        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDeviceSize size;

        void copyBuffer
        (
            VkBuffer src, 
            VkBuffer dst, 
            VkDeviceSize size,
            const Command & command
        );

        void createBuffer
        (
            VkDeviceSize size, 
            VkBufferUsageFlags usage, 
            VkMemoryPropertyFlags properties
        );


    };

    uint32_t findMemoryType
    (
        const Device & device,
        uint32_t typeFilter, 
        VkMemoryPropertyFlags properties
    );

}

#endif /* BUFFER */
