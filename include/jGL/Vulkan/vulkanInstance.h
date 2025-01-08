#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#ifndef ANDROID
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <version.h>
#include <jGL/Vulkan/vkDebug.h>

#include <jGL/jGL.h>

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/surface.h>
#include <jGL/Vulkan/swapchain.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/RenderPass/renderPass.h>
#include <jGL/Vulkan/framebuffer.h>
#include <jGL/Vulkan/pipeline.h>

#include <jGL/Vulkan/vkParticles.h>
#include <jGL/Vulkan/vkSpriteRenderer.h>
#include <jGL/Vulkan/vkShapeRenderer.h>
#include <jGL/Vulkan/Texture/vkTexture.h>

#include <jGL/Vulkan/Text/vkTextRenderer.h>

namespace jGL::Vulkan
{
    class vkTexture;

    class VulkanInstance : public jGLInstance
    {

    public:

        VulkanInstance(const DesktopDisplay & display);

        ~VulkanInstance();

        void finish()
        {
            closing = true; 
            vkDeviceWaitIdle(device.getVkDevice());
        }

        void text
        (
            std::string characters, 
            glm::vec2 position,
            float scale,
            glm::vec4 colour,
            glm::bvec2 centre = glm::bvec2(false,false)
        )
        {
            textRenderer->renderText
            (
                device,
                command,
                commandBuffer,
                currentFrame,
                characters, 
                position, 
                scale, 
                colour, 
                centre
            );
        }

        void clear() {/*TODO*/}

        void setMSAA(uint8_t samples) {/*TODO*/}

        void setClear(glm::vec4 colour) {/*TODO*/}
        void setProjection(glm::mat4 proj) {/*TODO*/}
        void setTextProjection(glm::mat4 p) {textRenderer->setProjection(p);}
        void setViewport(glm::vec4 view)
        {
            viewport.x = view.x;
            viewport.y = view.y;
            viewport.width = view.z;
            viewport.height = view.w;
        }

        std::shared_ptr<Particles> createParticles(size_t sizeHint) 
        { 
            return std::static_pointer_cast<Particles>(std::make_shared<vkParticles>(sizeHint)); 
        }
        std::shared_ptr<Texture> createTexture(std::filesystem::path imageFile, Texture::Type type) 
        { 
            return std::static_pointer_cast<Texture>
            (
                std::make_shared<vkTexture>
                (
                    device, 
                    command,
                    imageFile, 
                    type
                )
            );   
        }
        std::shared_ptr<Texture> createTexture(std::vector<std::byte> data, Texture::Type type) 
        { 
            TODO("VulkanInstance::createTexture from bytes undefined");
            return std::static_pointer_cast<Texture>
            (
                std::make_shared<vkTexture>
                (
                    device
                )
            );   
        }

        std::shared_ptr<SpriteRenderer> createSpriteRenderer(size_t sizeHint)
        {
            return std::static_pointer_cast<SpriteRenderer>
            (
                std::make_shared<vkSpriteRenderer>
                (
                    sizeHint
                )
            );
        }

        std::shared_ptr<ShapeRenderer> createShapeRenderer(size_t sizeHint)
        {
            return std::static_pointer_cast<ShapeRenderer>
            (
                std::make_shared<vkShapeRenderer>
                (
                    sizeHint
                )
            );
        }

        const Device & getDevice() const { return device; }
        const Command & getCommand() const { return command; }
        const VkInstance & getVkInstance() const { return instance; }
        const Swapchain & getSwapchain() const { return swapchain; }
        const RenderPass & getRenderPass() const { return renderPass; }
        const unsigned getConcurrentFrames() const { return concurrentFrames; }

        void beginFrame();
        void endFrame();

        void createSyncObjects();

    private:

        const unsigned concurrentFrames = 2;

        unsigned currentFrame = 0;

        bool midFrame = false;

        glm::vec4 clearColour = glm::vec4(1.0,1.0,1.0,1.0);

        std::vector<VkSemaphore> imageAvailableSemaphores, renderFinsihedSemaphores;
        std::vector<VkFence> framesFinished;

        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;

        std::vector<VkLayerProperties> availableLayers;

        Surface surface;
        Device device;
        uint32_t swapchainImageIndex = 0;
        Swapchain swapchain;
        Command command;
        RenderPass renderPass;
        VkCommandBuffer commandBuffer;
        std::vector<std::unique_ptr<Framebuffer>> framebuffers;

        std::unique_ptr<TextRenderer> textRenderer;

        VkRect2D scissor;
        VkViewport viewport;

        VkImage framebufferImage;
        VkImageView framebufferImageView;
        VkDeviceMemory framebufferDeviceMemory;

        void supportedValidationLayers(bool print = false);
        bool checkValidationLayerSupport();
        
    };
}

#endif /* VULKANINSTANCE_H */
