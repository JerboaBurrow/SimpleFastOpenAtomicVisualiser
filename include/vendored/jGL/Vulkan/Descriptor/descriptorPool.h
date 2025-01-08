#include <jGL/Vulkan/Device/device.h>
#include <vector>

namespace jGL::Vulkan
{
    class DescriptorPool
    {

    public:

        DescriptorPool
        (
            const Device & device, 
            const uint32_t concurrentFrames,
            std::vector<VkDescriptorType> descriptorTypes
        )
        : device(device)
        {
            std::vector<VkDescriptorPoolSize> sizes(descriptorTypes.size());
            
            for (unsigned i = 0; i < descriptorTypes.size(); i++)
            {
                sizes[i].type = descriptorTypes[i];
                sizes[i].descriptorCount = concurrentFrames;
            }


            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = sizes.size();
            poolInfo.pPoolSizes = sizes.data();
            poolInfo.maxSets = concurrentFrames*sizes.size();

            if (vkCreateDescriptorPool(device.getVkDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create descriptor pool");
            }
        }

        ~DescriptorPool()
        {
            vkDestroyDescriptorPool(device.getVkDevice(), descriptorPool, nullptr);
        }

        const VkDescriptorPool & getVkDescriptorPool() const { return descriptorPool; }

    private:

        const Device & device;

        VkDescriptorPool descriptorPool;


    };
}