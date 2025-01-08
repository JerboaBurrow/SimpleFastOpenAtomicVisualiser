#ifndef VERTEXBUFFER
#define VERTEXBUFFER

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/buffer.h>
#include <memory>

#include <cstddef>

#define GLM_FORCE_RADIANS
// will mostly auto align buffer data
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL::Vulkan
{

    class VertexBufferObject
    {

        /*
        
            Abstract base class representing buffer objects

                Use derived types, e.g. VertexBuffer<glm::vec4> to
                    represent a vertex buffer of a single vec4, that
                    can be used as e.g. layout(location=0) vec4 colour
                    or something

                Templated concrete types are specialised, following the
                    constructor signature:

                        VertexBuffer<T>
                        (
                            const Device & device,
                            const Command & command,
                            std::vector<T> data,
                            VkVertexInputRate rate,
                            uint32_t binding = 0,
                            uint32_t location = 0
                        );
        
        */
    
    public:

        VertexBufferObject(const Device & device)
        :
        device(device)
        {}

        // an implementation must define these two layout properties
        virtual std::vector<VkVertexInputBindingDescription> getBindingDescription() = 0;
        virtual std::vector<VkVertexInputAttributeDescription> getArrtibuteDescriptions() = 0;

        void setBinding(uint32_t b) { binding = b; }
        void setLocation(uint32_t l) { location = l; }

        const VkBuffer & getVkBuffer() const { return buffer->getVkBuffer(); }

    protected:

        const Device & device;

        VkVertexInputRate inputRate;

        uint32_t binding, location;

        std::unique_ptr<Buffer> buffer;

    };

    template <class T>
    class VertexBuffer : public VertexBufferObject
    {
    
    public:

        VertexBuffer
        (
            const Device & device,
            const Command & command,
            std::vector<T> data,
            VkVertexInputRate rate,
            uint32_t binding = 0,
            uint32_t location = 0
        )
        : VertexBufferObject(device)
        {

            this->inputRate = rate; 
            this->binding = binding; 
            this->location = location;

            VkDeviceSize bufferSize = sizeof(T)*data.size();

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            // create normal buffer

            buffer = std::make_unique<Buffer>
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

            buffer->copy(stagingBuffer, command);
        }

        void subData
        (
            const Device & device,
            const Command & command,
            std::vector<T> data
        )
        {

            VkDeviceSize bufferSize = sizeof(T)*data.size();

            if (bufferSize > buffer->getVkDeviceSize())
            {
                buffer = std::make_unique<Buffer>
                (
                    device,
                    bufferSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
            }

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            buffer->copy(stagingBuffer, command);
        }

        std::vector<VkVertexInputBindingDescription> getBindingDescription();
        std::vector<VkVertexInputAttributeDescription> getArrtibuteDescriptions();

    private:

        

    };

    template <>
    class VertexBuffer<glm::vec4> : public VertexBufferObject
    {
    
    public:

        VertexBuffer<glm::vec4>
        (
            const Device & device,
            const Command & command,
            std::vector<glm::vec4> data,
            VkVertexInputRate rate,
            uint32_t binding = 0,
            uint32_t location = 0
        )
        : VertexBufferObject(device)
        {

            // hate this, take a look to abstract

            this->inputRate = rate; 
            this->binding = binding; 
            this->location = location;

            VkDeviceSize bufferSize = sizeof(glm::vec4)*data.size();

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            // create normal buffer

            buffer = std::make_unique<Buffer>
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

            buffer->copy(stagingBuffer, command);
        }

        void subData
        (
            const Device & device,
            const Command & command,
            std::vector<glm::vec4> data
        )
        {

            VkDeviceSize bufferSize = sizeof(glm::vec4)*data.size();

            if (bufferSize > buffer->getVkDeviceSize())
            {
                buffer = std::make_unique<Buffer>
                (
                    device,
                    bufferSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
            }

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            buffer->copy(stagingBuffer, command);
        }
        

        std::vector<VkVertexInputBindingDescription> getBindingDescription()
        {
            std::vector<VkVertexInputBindingDescription> bindingDescription{};
            bindingDescription.resize(1);
            bindingDescription[0].binding = binding;
            bindingDescription[0].stride = sizeof(glm::vec4);
            bindingDescription[0].inputRate = inputRate;
            return bindingDescription;
        }

        std::vector<VkVertexInputAttributeDescription> getArrtibuteDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
            attributeDescriptions.resize(1);
            // vertex binding, 0
            attributeDescriptions[0].binding = binding;
            // layout(location = 0)
            attributeDescriptions[0].location = location;
            // we have a vec4, so rgba
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            // simply a glm::vec4, so no offset here
            attributeDescriptions[0].offset = 0;

            return attributeDescriptions;
        }

    };

    template <>
    class VertexBuffer<glm::vec2> : public VertexBufferObject
    {
    
    public:

        VertexBuffer<glm::vec2>
        (
            const Device & device,
            const Command & command,
            std::vector<glm::vec2> data,
            VkVertexInputRate rate,
            uint32_t binding = 0,
            uint32_t location = 0
        )
        : VertexBufferObject(device)
        {

            // hate this, take a look to abstract

            this->inputRate = rate; 
            this->binding = binding; 
            this->location = location;

            VkDeviceSize bufferSize = sizeof(glm::vec2)*data.size();

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            // create normal buffer

            buffer = std::make_unique<Buffer>
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );

            buffer->copy(stagingBuffer, command);
        }

        void subData
        (
            const Device & device,
            const Command & command,
            std::vector<glm::vec2> data
        )
        {

            VkDeviceSize bufferSize = sizeof(glm::vec2)*data.size();

            if (bufferSize > buffer->getVkDeviceSize())
            {
                buffer = std::make_unique<Buffer>
                (
                    device,
                    bufferSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );
            }

            Buffer stagingBuffer
            (
                device,
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            stagingBuffer.copyMemory
            (
                0,
                bufferSize,
                data.data()
            );

            buffer->copy(stagingBuffer, command);
        }
        

        std::vector<VkVertexInputBindingDescription> getBindingDescription()
        {
            std::vector<VkVertexInputBindingDescription> bindingDescription{};
            bindingDescription.resize(1);
            bindingDescription[0].binding = binding;
            bindingDescription[0].stride = sizeof(glm::vec2);
            bindingDescription[0].inputRate = inputRate;
            return bindingDescription;
        }

        std::vector<VkVertexInputAttributeDescription> getArrtibuteDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
            attributeDescriptions.resize(1);
            // vertex binding, 0
            attributeDescriptions[0].binding = binding;
            // layout(location = 0)
            attributeDescriptions[0].location = location;
            // we have a vec2, so rg
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            // simply a glm::vec2, so no offset here
            attributeDescriptions[0].offset = 0;

            return attributeDescriptions;
        }

    };
}

#endif /* VERTEXBUFFER */

/*

    A note on how attributes and bindings function

    https://gist.github.com/SaschaWillems/428d15ed4b5d71ead462bc63adffa93a

    Shader

    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inColor;
    layout (location = 2) in vec3 inPosB;
    layout (location = 3) in vec3 inColorB;

    ...

    Application

    // Input state
    std::array<VkVertexInputBindingDescription,2> vertexInputBinding = {};
    vertexInputBinding[0].binding = 0;
    vertexInputBinding[0].stride = sizeof(Vertex);
    vertexInputBinding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBinding[1].binding = 1;
    vertexInputBinding[1].stride = sizeof(Vertex);
    vertexInputBinding[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Inpute attribute bindings describe shader attribute locations and memory layouts
    std::array<VkVertexInputAttributeDescription, 4> vertexInputAttributs;
    vertexInputAttributs[0].binding = 0;
    vertexInputAttributs[0].location = 0;
    vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[0].offset = offsetof(Vertex, position);
    vertexInputAttributs[1].binding = 0;
    vertexInputAttributs[1].location = 1;
    vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[1].offset = offsetof(Vertex, color);

    vertexInputAttributs[2].binding = 1;
    vertexInputAttributs[2].location = 2;
    vertexInputAttributs[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[2].offset = offsetof(Vertex, position);
    vertexInputAttributs[3].binding = 1;
    vertexInputAttributs[3].location = 3;
    vertexInputAttributs[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[3].offset = offsetof(Vertex, color);

    // Vertex input state used for pipeline creation
    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 2;
    vertexInputState.pVertexBindingDescriptions = vertexInputBinding.data();
    vertexInputState.vertexAttributeDescriptionCount = 4;
    vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();

    // Bind
    vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &vertices.buffer, offsets);
    vkCmdBindVertexBuffers(drawCmdBuffers[i], 1, 1, &verticesB.buffer, offsets);

*/