#include "PCH.h"

#include "Sampler.h"

Sampler::Sampler(VkFilter min, VkFilter mag, VkSamplerAddressMode modeU, VkSamplerAddressMode modeV,
	VkSamplerAddressMode modeW, VkSamplerMipmapMode mipMode, float lodBias, VkBool32 anisotropy, float maxAnisotropy,
	VkBool32 compare, VkCompareOp compareOp, float minLod, float maxLod, VkBorderColor border,
	VkSamplerCreateFlags flags)
{
	VkSamplerCreateInfo info{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.flags = flags,
		.magFilter = mag,
		.minFilter = min,
		.mipmapMode = mipMode,
		.addressModeU = modeU,
		.addressModeV = modeV,
		.addressModeW = modeW,
		.mipLodBias = lodBias,
		.anisotropyEnable = anisotropy,
		.maxAnisotropy = maxAnisotropy,
		.compareEnable = compare,
		.compareOp = compareOp,
		.minLod = minLod,
		.maxLod = maxLod,
		.borderColor = border,
		.unnormalizedCoordinates = VK_FALSE };

	VkCall(vkCreateSampler(Instance::Device(), &info, nullptr, &m_Sampler));
}

Sampler::~Sampler() { vkDestroySampler(Instance::Device(), m_Sampler, nullptr); }

Sampler::Sampler(Sampler&& other)
{
	m_Sampler = other.m_Sampler;
	other.m_Sampler = VK_NULL_HANDLE;
}

Sampler& Sampler::operator=(Sampler&& other)
{
	this->~Sampler();

	m_Sampler = other.m_Sampler;
	other.m_Sampler = VK_NULL_HANDLE;

	return *this;
}
