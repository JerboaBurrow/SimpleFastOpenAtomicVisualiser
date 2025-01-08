#ifndef VKFONT_H
#define VKFONT_H

#include <jGL/glyph.h>
#include <jGL/font.h>

#include <jGL/Vulkan/Device/device.h>
#include <jGL/Vulkan/Command/command.h>
#include <jGL/Vulkan/Texture/vkTexture.h>
#include <jGL/Vulkan/Texture/vkSampler.h>

#include <algorithm>


namespace jGL::Vulkan
{
    class vkFont : public Font 
    {

    public:

        vkFont(const Device & device, const Command & command, uint8_t w)
        : Font(w), device(device), command(command)
        {
            upload(bw, bh);
        }

        ~vkFont() {}

        const VkImageView & getGlyphView() const { return view; }

    private:

        const Device & device;
        const Command & command;

        VkImageView view;

        void upload(uint16_t w, uint16_t h);
    };
}

#endif /* VKFONT_H */
