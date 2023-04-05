#pragma once
#include "framework/kvulkan.h"
namespace kvs
{
    class LogicDevice;
    class Uniform;

    class Descriptor {
    public:
        Descriptor(LogicDevice& logic_device);

        void CreateSetLayout(VkDescriptorType type, VkShaderStageFlags stageFlags);
        void CreateDescriptor(Uniform& unifrom);
        void CleanUpDescriptor();
        std::vector<VkDescriptorSet> m_Sets;
        VkDescriptorSetLayout m_DesSetLayout;
    private:
        VkDevice& m_Device;
        VkDescriptorPool m_Pool;
    };

    
} // namespace kvs
