#include "framework/Image/image.h"
#include "image.h"

#include <stb_image.h>
namespace kvs
{

    void Texture::CreateTextureImage(const char *filename)
    {
		int texWidth, texHeight, texChannels;
		auto pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if(!pixels){
			throw std::runtime_error("failed to read texture");
		}
    }

} // namespace kvs
