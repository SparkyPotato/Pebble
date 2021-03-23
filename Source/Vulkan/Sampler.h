#pragma once

#include "Instance.h"

class Sampler
{
public:
	Sampler() = default;
	Sampler(VkFilter min, VkFilter mag, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkSamplerMipmapMode mipMode = VK_SAMPLER_MIPMAP_MODE_LINEAR, float lodBias = 0.f,
		VkBool32 anisotropy = VK_FALSE, float maxAnisotropy = 0.f, VkBool32 compare = VK_FALSE,
		VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS, float minLod = 0.f, float maxLod = 0.f,
		VkBorderColor border = VK_BORDER_COLOR_INT_OPAQUE_BLACK, VkSamplerCreateFlags flags = 0);
	~Sampler();

	Sampler(const Sampler& other) = delete;
	Sampler& operator=(const Sampler& other) = delete;

	Sampler(Sampler&& other);
	Sampler& operator=(Sampler&& other);

private:
	VkSampler m_Sampler = VK_NULL_HANDLE;
};
