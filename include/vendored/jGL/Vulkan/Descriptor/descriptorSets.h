#ifndef DESCRIPTORSETS
#define DESCRIPTORSETS

#include <jGL/Vulkan/vkDebug.h>
#include <jGL/Vulkan/Descriptor/descriptorPool.h>


namespace jGL::Vulkan
{
    template <class T>
    class DescriptorSets;

    template <>
    class DescriptorSets<VkDescriptorBufferInfo>
    {

    public:

        DescriptorSets() = default;

        DescriptorSets
        (
            const Device & device,
            const DescriptorPool & pool,
            std::vector<VkDescriptorSetLayout> & layouts,
            std::vector<std::vector<VkDescriptorBufferInfo>> & infos,
            std::vector<std::vector<VkDescriptorType>> & types
        )
        {
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = pool.getVkDescriptorPool();
            allocInfo.descriptorSetCount = layouts.size();
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(layouts.size());
            if (vkAllocateDescriptorSets(device.getVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate image descriptor sets");
            }


            for (size_t i = 0; i < layouts.size(); i++)
            {
                for (size_t j = 0; j < infos[i].size(); j++)
                {
                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = descriptorSets[i];
                    descriptorWrite.dstBinding = j;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = types[i][j];
                    descriptorWrite.descriptorCount = 1;

                    descriptorWrite.pBufferInfo = &infos[i][j];

                    vkUpdateDescriptorSets(device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
                }
            }
        }

        const VkDescriptorSet & getVkDescriptorSet(size_t i) const { return descriptorSets[i]; }
        const std::vector<VkDescriptorSet> & getVkDescriptorSets() const { return descriptorSets; }

    private:

        std::vector<VkDescriptorSet> descriptorSets;

    };

    template <>
    class DescriptorSets<VkDescriptorImageInfo>
    {

    public:

        DescriptorSets() = default;

        DescriptorSets
        (
            const Device & device,
            const DescriptorPool & pool,
            std::vector<VkDescriptorSetLayout> & layouts,
            std::vector<std::vector<VkDescriptorImageInfo>> & infos,
            std::vector<std::vector<VkDescriptorType>> & types
        )
        {
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = pool.getVkDescriptorPool();
            allocInfo.descriptorSetCount = layouts.size();
            allocInfo.pSetLayouts = layouts.data();
            // number of layouts is number of frames...
            descriptorSets.resize(layouts.size());
            if (vkAllocateDescriptorSets(device.getVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to allocate image descriptor sets");
            }

            for (size_t i = 0; i < layouts.size(); i++)
            {
                for (size_t j = 0; j < infos[i].size(); j++)
                {
                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = descriptorSets[i];
                    descriptorWrite.dstBinding = j;
                    descriptorWrite.dstArrayElement = 0;

                    descriptorWrite.descriptorType = types[i][j];
                    descriptorWrite.descriptorCount = 1;

                    descriptorWrite.pImageInfo = &infos[i][j];

                    vkUpdateDescriptorSets(device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
                }
            }
        }
        
        const VkDescriptorSet & getVkDescriptorSet(size_t i) const { return descriptorSets[i]; }
        const std::vector<VkDescriptorSet> & getVkDescriptorSets() const { return descriptorSets; }

    private:

        std::vector<VkDescriptorSet> descriptorSets;

    };
}

#endif /* DESCRIPTORSETS */
