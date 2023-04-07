#pragma once
#include "framework/kvulkan.h"
namespace kvs
{
    class LogicDevice;
    class Uniform;
    class TextureImage;

    class Descriptor {
    public:
        Descriptor(LogicDevice& logic_device);

        void CreateSetLayout(VkDescriptorType type, VkShaderStageFlags stageFlags);
        void CreateDescriptor(Uniform& unifrom, TextureImage& image);
        void CleanUpDescriptor();
        void UpdateDescriptorSet(Uniform& unifrom, TextureImage& image);
        std::vector<VkDescriptorSet> m_Sets;
        VkDescriptorSetLayout m_DesSetLayout;
    private:
        VkDevice& m_Device;
        VkDescriptorPool m_Pool;
    };

    
} // namespace kvs
