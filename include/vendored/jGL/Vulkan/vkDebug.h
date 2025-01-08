#ifndef VKDEBUG
#define VKDEBUG

#include <vector>
#include <iostream>
#include <string>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <jGL/debug.h>

#ifdef VALIDATION
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

#ifdef VERBOSE
    const bool verboseValidationLayers = true;
#else
    const bool verboseValidationLayers = false;
#endif

const std::vector<const char *> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
    void                                       * pUserData
)
{
    /*
        messageSeverity can be

            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:    Diagnostic message
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:       Informational message like the creation of a resource
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:    Message about behavior that is not necessarily an error, but very likely a bug in your application
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:      Message about behavior that is invalid and may cause crashes

        messageType can be

            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:        Some event has happened that is unrelated to the specification or performance
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:     Something has happened that violates the specification or indicates a possible mistake
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:    Potential non-optimal use of Vulkan

    */
    if (verboseValidationLayers)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }
    else
    { 
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
        {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
            #ifdef EXCEPT_ON_VALIDATION
                throw std::runtime_error("Vulkan validation warning or error encountered");
            #endif
        }
    }

    // abort triggering call
    return VK_FALSE;
}

[[maybe_unused]] static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo) {
    createInfo = {};
    createInfo.sType =              VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback =    debugCallback;
    createInfo.pUserData =          nullptr; // Optional
    createInfo.pNext =              nullptr;
    createInfo.flags =              0;
}

void vkError(VkResult result, std::string context = "unspecified");

#endif /* VKDEBUG */
