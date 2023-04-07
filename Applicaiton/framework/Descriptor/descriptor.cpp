#include "framework/Descriptor/descriptor.h"
#include "descriptor.h"
#include "LogicDevice/logicdevice.h"
#include "framework/Uniform/uniform.h"
#include "framework/Buffer/buffer.h"
#include "framework/Image/image.h"
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

        VkDescriptorSetLayoutBinding samplerBindInfo{};
        samplerBindInfo.binding = 1;
        samplerBindInfo.descriptorCount = 1;
        samplerBindInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerBindInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerBindInfo.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings{ bindInfo, samplerBindInfo };
        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = bindings.size();
        createInfo.pBindings = bindings.data();

        vkCreateDescriptorSetLayout(m_Device, &createInfo, nullptr, &m_DesSetLayout);
    }

    void kvs::Descriptor::CreateDescriptor(Uniform& unifrom, TextureImage& image)
    {
        std::array<VkDescriptorPoolSize, 2> pool_size{};
        pool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size[0].descriptorCount = frame_in_flight;

        pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_size[1].descriptorCount = frame_in_flight;

        VkDescriptorPoolCreateInfo createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createinfo.maxSets = frame_in_flight;
        createinfo.poolSizeCount = pool_size.size();
        createinfo.pPoolSizes = pool_size.data();

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

        UpdateDescriptorSet(unifrom, image);
    }

    void Descriptor::CleanUpDescriptor()
    {
        vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
        vkDestroyDescriptorSetLayout(m_Device, m_DesSetLayout, nullptr);
    }

    void Descriptor::UpdateDescriptorSet(Uniform& unifrom, TextureImage& image)
    {
        for (int i = 0; i < frame_in_flight; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = unifrom.m_Buffers[i].GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(VertexTransformMat);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = image.m_Image.m_ImageView;
            imageInfo.sampler = image.m_Sampler;


            std::array<VkWriteDescriptorSet, 2> writes{};
            writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet = m_Sets[i];
            writes[0].dstBinding = 0;
            writes[0].dstArrayElement = 0;
            writes[0].descriptorCount = 1;
            writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[0].pBufferInfo = &bufferInfo;

            writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet = m_Sets[i];
            writes[1].dstBinding = 1;
            writes[1].dstArrayElement = 0;
            writes[1].descriptorCount = 1;
            writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[1].pImageInfo = &imageInfo;


            vkUpdateDescriptorSets(m_Device, writes.size(), writes.data(), 0, nullptr);
        }
    }

} // namespace kvs
