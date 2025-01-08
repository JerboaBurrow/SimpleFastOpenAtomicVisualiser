#ifndef VKSHADER
#define VKSHADER

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include <jGL/shader.h>

#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

namespace jGL::Vulkan
{

    class vkShader : public Shader
    {

        /*

            Shaderc macro definitions
                // Like -DMY_DEFINE=1
                options.AddMacroDefinition("MY_DEFINE", "1");

        */

    public:

        vkShader(const VkDevice & d, const char * vs, const char * fs)
        : Shader(vs, fs), device(d)
        {
            
            compile();

            createShaderModules(device);

        }

        ~vkShader();
        
        std::vector<VkPipelineShaderStageCreateInfo> shaderStage() const;

        void use(){}

    private:

        const VkDevice & device;

        VkShaderModule vertexModule, fragmentModule;

        std::vector<uint32_t> vertexSource, fragmentSource;

        void compile();

        std::vector<char> readSPIRV(const std::string & filename);
        void createShaderModules(const VkDevice & device);

        // Returns GLSL vkShader source text after preprocessing.
        std::string preprocessShader
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options
        );

        // Compiles a vkShader to SPIR-V assembly. Returns the assembly text
        // as a string.
        std::string compileToAssembly
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options,
            bool optimize = false
        );


        // Compiles a vkShader to a SPIR-V binary. Returns the binary as
        // a vector of 32-bit words.
        std::vector<uint32_t> compileSPIRV
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options,
            bool optimize = false
        );
    };
}

#endif /* VKSHADER */
