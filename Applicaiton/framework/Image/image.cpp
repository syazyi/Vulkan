#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
#include "framework/Buffer/buffer.h"
#include "CommandBuffer/commandbuffer.h"
namespace kvs
{
	Texture::Texture(const char* filename)
	{
		CreateTextureImage(filename);
	}

	Texture::~Texture()
	{
		//stbi_image_free(pixels);
	}

	void Texture::DestroyTexture()
	{
		stbi_image_free(pixels);
	}

	VkDeviceSize Texture::GetDeviceSize()
	{
		VkDeviceSize a = texHeight * texWidth * 4;
		return a;
	}

	void Texture::CreateTextureImage(const char* filename)
	{
		if (pixels != nullptr) {
			return;
		}
		pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels) {
			throw std::runtime_error("failed to read texture");
		}

		mipLevelCount = std::floor(std::log2(std::max(texWidth, texHeight))) + 1;
	}


	void Image::Createimage(VkDevice& device, VkPhysicalDevice& pDevice, uint32_t width, uint32_t height, uint32_t mipCounts, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags flags)
	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = format;
		createInfo.extent.width = width;
		createInfo.extent.height = height;
		createInfo.extent.depth = 1;
		createInfo.mipLevels = mipCounts;
		createInfo.arrayLayers = 1;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = tiling;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (vkCreateImage(device, &createInfo, nullptr, &m_Image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image");
		}

		VkMemoryRequirements require{};
		vkGetImageMemoryRequirements(device, m_Image, &require);

		VkMemoryAllocateInfo allInfo{};
		allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allInfo.allocationSize = require.size;

		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(pDevice, &memProperties);
		uint32_t i = 0;
		for (; i < memProperties.memoryTypeCount; i++) {
			if ((require.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags)){
				break;
			}
		}
		allInfo.memoryTypeIndex = i;
		vkAllocateMemory(device, &allInfo, nullptr, &m_ImageMemory);
		vkBindImageMemory(device, m_Image, m_ImageMemory, 0);
	}

	void Image::ClearUp(VkDevice& device)
	{
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkFreeMemory(device, m_ImageMemory, nullptr);
		vkDestroyImage(device, m_Image, nullptr);
	}

	void Image::CreateImageView(VkDevice& device, VkFormat format, uint32_t mipCounts)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		//createInfo.components;
		createInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.levelCount = mipCounts;

		if (vkCreateImageView(device, &createInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view");
		}
	}

	TextureImage::TextureImage(LogicDevice& logic_device) : m_Device(logic_device.GetLogicDevice()), m_GraphicQueue(logic_device.m_GraphicsQueue)
	{

	}

	void TextureImage::CreateTextureImage(PhysicalDevice& pDevice, Command& command, Texture& texture)
	{
		VkDeviceSize size = texture.GetDeviceSize();

		m_MipmapLevelCount = texture.mipLevelCount;

		VkFormatProperties formatPro;
		vkGetPhysicalDeviceFormatProperties(pDevice.GetPhysicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &formatPro);

		if (!(formatPro.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting");
		}

		Buffer m_StagingBuffer;
		m_StagingBuffer.CreateBuffer(m_Device, pDevice.GetPhysicalDevice(), size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(m_Device, m_StagingBuffer.GetMemory(), 0, size, 0, &data);
		memcpy(data, texture.pixels, size);
		vkUnmapMemory(m_Device, m_StagingBuffer.GetMemory());


		m_Image.Createimage(m_Device, pDevice.GetPhysicalDevice(), texture.texWidth, texture.texHeight, texture.mipLevelCount,  VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		auto cmdBuffer = command.BeginSingleTimeCommands();

		TransitionImageLayout(cmdBuffer, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyTextureImage(cmdBuffer, m_StagingBuffer.GetBuffer(), texture.texWidth, texture.texHeight);
		CreateMipMaps(cmdBuffer, texture);
		//TransitionImageLayout(cmdBuffer, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		command.EndSingleTimeCommands(cmdBuffer, m_GraphicQueue);
		m_StagingBuffer.FreeBufferAndMemory(m_Device);
	}

	void TextureImage::TransitionImageLayout(VkCommandBuffer& cmdBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Image.m_Image;
		barrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = m_MipmapLevelCount;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;

		VkPipelineStageFlags srcStageMask = 0;
		VkPipelineStageFlags dstStageMask = 0;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
			barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;

			srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;
		}

		vkCmdPipelineBarrier(cmdBuffer,
			srcStageMask, dstStageMask, 0, 
			0,nullptr, 
			0, nullptr,
			1, &barrier);

	}

	void TextureImage::CopyTextureImage(VkCommandBuffer& cmdBuffer, VkBuffer& srcBuffer, uint32_t width, uint32_t height)
	{

		VkBufferImageCopy regions{};
		regions.bufferOffset = 0;
		regions.bufferRowLength = 0;
		regions.bufferImageHeight = 0;
		regions.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		regions.imageSubresource.mipLevel = 0;
		regions.imageSubresource.layerCount = 1;
		regions.imageSubresource.baseArrayLayer = 0;

		regions.imageExtent = {width, height, 1};
		regions.imageOffset = { 0, 0, 0 };

		vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, m_Image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &regions);

	}

	void TextureImage::CleanUp()
	{
		m_Image.ClearUp(m_Device);
		vkDestroySampler(m_Device, m_Sampler, nullptr);
	}

	void TextureImage::CreateImageView(VkFormat format)
	{
		m_Image.CreateImageView(m_Device, format, m_MipmapLevelCount);
	}

	void TextureImage::CreateSampler(PhysicalDevice& physical)
	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.mipLodBias = 0.0f;
		createInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical.GetPhysicalDevice(), &properties);
		createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = static_cast<float>(m_MipmapLevelCount);
		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;

		if (vkCreateSampler(m_Device, &createInfo, nullptr, &m_Sampler) != VK_SUCCESS) {
			throw "failed to create sampler.";
		}
	}

	void TextureImage::CreateMipMaps(VkCommandBuffer& cmdBuffer, Texture& texture)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Image.m_Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int mipWidth = texture.texWidth;
		int mipHeight = texture.texHeight;

		for (uint32_t i = 1; i < m_MipmapLevelCount; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
				0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};

			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.srcSubresource.mipLevel = i - 1;
			
			blit.dstOffsets[0] = {0, 0, 0};
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			blit.dstSubresource.mipLevel = i;

			vkCmdBlitImage(cmdBuffer, m_Image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
				m_Image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
		
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, 0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) {
				mipWidth /= 2;
			}
			if (mipHeight > 1) {
				mipHeight /= 2;
			}
		}

		barrier.subresourceRange.baseMipLevel = m_MipmapLevelCount - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
			0, 0, nullptr, 
			0, nullptr, 
			1, &barrier);
	}

} // namespace kvs
