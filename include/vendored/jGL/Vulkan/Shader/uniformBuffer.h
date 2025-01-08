#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/buffer.h>
#include <memory>

namespace jGL::Vulkan
{
    class UniformBufferObject
    {

    public:

        UniformBufferObject(const Device & device)
        :
        device(device)
        {}

        void setBinding(uint32_t b) { binding = b; }

        virtual VkDescriptorSetLayoutBinding getLayout() = 0;
        virtual VkDescriptorBufferInfo getBufferInfo() = 0;

    protected:

        const Device & device;

        VkDeviceSize size;

        uint32_t binding = 0;
        VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL;

        std::unique_ptr<Buffer> buffer;

    };

    template <class T>
    class UniformBuffer : public UniformBufferObject
    {
    
    public:

        UniformBuffer
        (
            const Device & device,
            T data,
            uint32_t binding = 0,
            VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL
        )
        : UniformBufferObject(device)
        {
            this->binding = binding;
            this->stage = stage;

            size = sizeof(T);
            
            buffer = std::make_unique<Buffer>
            (
                device,
                size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            buffer->copyMemory
            (
                0,
                size,
                &data,
                0
            );
        }

        VkDescriptorSetLayoutBinding getLayout() 
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = binding;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = stage;
            return uboLayoutBinding;
        }

        VkDescriptorBufferInfo getBufferInfo()
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = buffer->getVkBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(T);
            return bufferInfo;
        }

        void set(T data)
        {
            buffer->copyMemory
            (
                0,
                size,
                &data,
                0
            );
        }

    private:


    };
}