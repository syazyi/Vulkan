#include "framework/Descriptor/descriptor.h"
#include "descriptor.h"
#include "LogicDevice/logicdevice.h"
#include "framework/Uniform/uniform.h"
#include "framework/Buffer/buffer.h"
namespace kvs
{
    Descriptor::Descriptor(LogicDevice& logic_device) : m_Device(logic_device.GetLogicDevice())
    {
        m_Sets.resize(frame_in_flight);
    }

    void Descriptor::CreateSetLayout(VkDescriptorType type, VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding bindInfo{};
        bindInfo.binding = 0;
        bindInfo.descriptorCount = 1;
        bindInfo.descriptorType = type;
        bindInfo.stageFlags = stageFlags;

        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = 1;
        createInfo.pBindings = &bindInfo;

        vkCreateDescriptorSetLayout(m_Device, &createInfo, nullptr, &m_DesSetLayout);
    }

    void kvs::Descriptor::CreateDescriptor(Uniform& unifrom)
    {
        VkDescriptorPoolSize pool_size{};
        pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size.descriptorCount = frame_in_flight;

        VkDescriptorPoolCreateInfo createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createinfo.maxSets = frame_in_flight;
        createinfo.poolSizeCount = 1;
        createinfo.pPoolSizes = &pool_size;

        if (vkCreateDescriptorPool(m_Device, &createinfo, nullptr, &m_Pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool");
        }

        std::vector<VkDescriptorSetLayout> layouts(frame_in_flight, m_DesSetLayout);
        VkDescriptorSetAllocateInfo allInfo{};
        allInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allInfo.descriptorPool = m_Pool;
        allInfo.descriptorSetCount = frame_in_flight;
        allInfo.pSetLayouts = layouts.data();


        if (vkAllocateDescriptorSets(m_Device, &allInfo, m_Sets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor set");
        }

        for (int i = 0; i < frame_in_flight; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = unifrom.m_Buffers[i].GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(VertexTransformMat);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_Sets[i];
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(m_Device, 1, &write, 0, nullptr);
        }
    }

    void Descriptor::CleanUpDescriptor()
    {
        vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, m_DesSetLayout, nullptr);
    }

} // namespace kvs
