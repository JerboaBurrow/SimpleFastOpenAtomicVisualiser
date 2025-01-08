#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <jGL/OpenGL/gl.h>
#include <jGL/texture.h>

#include <stdexcept>
#include <filesystem>

namespace jGL::GL
{

    class glTexture : public Texture
    {

    public:

        glTexture()
        {
            glGenTextures(1, &id);
        }

        ~glTexture()
        {
            glDeleteTextures(1, &id);
        }

        virtual void upload(std::vector<std::byte> * data) {};

        inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }

        inline void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

        void bind(unsigned binding) 
        {
            glActiveTexture(GL_TEXTURE0+binding);
            unbind();
            bind();
            textureUnit = binding;
        }

        void generateMipMaps() {bind(); glGenerateMipmap(GL_TEXTURE_2D);}

    protected:

        GLuint id;
        unsigned textureUnit;

        std::vector<std::byte> load_image(std::filesystem::path imageFilePath)
        {
            unsigned char * pixels = stbi_load(imageFilePath.generic_string().c_str(), &width, &height, &channels, 0);
        
            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture: "+imageFilePath.generic_string());
            }
            std::byte * bytes = reinterpret_cast<std::byte*>(pixels);

            size_t dim = width*height;
            if (channels > 0) { dim *= channels; }
            std::vector<std::byte> vdata(bytes, bytes+dim);
            stbi_image_free(pixels);
            return vdata;
        }

        std::vector<std::byte> load_image(std::vector<std::byte> imageFile)
        {
            unsigned char * chData = reinterpret_cast<unsigned char*>(imageFile.data());
            unsigned char * pixels = stbi_load_from_memory(chData, imageFile.size(), &width, &height, &channels, 4);
            if (!pixels)
            {
                throw std::runtime_error("Failed to load texture from memory");
            }
            std::byte * bytes = reinterpret_cast<std::byte*>(pixels);

            size_t dim = width*height;
            if (channels > 0) { dim *= channels; }
            std::vector<std::byte> vdata(bytes, bytes+dim);
            stbi_image_free(pixels);
            return vdata;
        }

    };

    class glTexture2DRGB : public glTexture
    {

    public:

        glTexture2DRGB(std::filesystem::path imageFile)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(imageFile);
            create(width, height, channels);
            upload(pixels);
        }

        glTexture2DRGB(std::vector<std::byte> data)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(data);
            create(width, height, channels);
            upload(pixels);
        }

        void create(int width, int height, int channels);
        void upload(std::vector<std::byte> pixels);
    };

    class glTexture2DRGBA : public glTexture
    {

    public:

        glTexture2DRGBA(std::filesystem::path imageFile)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(imageFile);
            create(width, height, channels);
            upload(pixels);
        }
    
        glTexture2DRGBA(std::vector<std::byte> data)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(data);
            create(width, height, channels);
            upload(pixels);
        }

        void create(int width, int height, int channels);
        void upload(std::vector<std::byte> pixels);
    };

    class glTexture2DByte : public glTexture
    {

    public:

        glTexture2DByte(std::filesystem::path imageFile)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(imageFile);
            create(width, height);
            upload(pixels);
        }

        glTexture2DByte(std::vector<std::byte> data)
        : glTexture()
        {
            std::vector<std::byte> pixels = load_image(data);
            create(width, height);
            upload(pixels);
        }

        glTexture2DByte(std::vector<std::byte> pixels, int width, int height)
        : glTexture()
        {
            create(width, height);
            upload(pixels);
        }

        void create(int width, int height);
        void upload(std::vector<std::byte> pixels);
        
    };
}

#endif /* GLTEXTURE_H */
