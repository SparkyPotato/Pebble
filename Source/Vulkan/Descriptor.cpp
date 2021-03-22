#include "PCH.h"

#include "Descriptor.h"

#include "PipelineLayout.h"

DescriptorSet::DescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) : m_Pool(pool)
{
	VkDescriptorSetAllocateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = m_Pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout };

	VkCall(vkAllocateDescriptorSets(Instance::Device(), &info, &m_Set));
}

DescriptorSet::~DescriptorSet() { vkFreeDescriptorSets(Instance::Device(), m_Pool, 1, &m_Set); }

DescriptorSet::DescriptorSet(DescriptorSet&& other)
{
	m_Set = other.m_Set;
	other.m_Set = VK_NULL_HANDLE;
	m_Pool = other.m_Pool;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other)
{
	this->~DescriptorSet();

	m_Set = other.m_Set;
	other.m_Set = VK_NULL_HANDLE;
	m_Pool = other.m_Pool;

	return *this;
}

DescriptorPool::DescriptorPool(std::span<VkDescriptorPoolSize> sizes, u32 maxSets)
{
	VkDescriptorPoolCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.poolSizeCount = u32(sizes.size()),
		.pPoolSizes = sizes.data() };

	VkCall(vkCreateDescriptorPool(Instance::Device(), &info, nullptr, &m_Pool));
}

DescriptorSet DescriptorPool::Allocate(const PipelineLayout& layout, u32 layoutIndex) 
{
	return DescriptorSet(m_Pool, layout.GetSetLayout(layoutIndex));
}

DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(Instance::Device(), m_Pool, nullptr);
}

DescriptorPool::DescriptorPool(DescriptorPool&& other)
{
	m_Pool = other.m_Pool;
	other.m_Pool = VK_NULL_HANDLE;
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other)
{
	this->~DescriptorPool();

	m_Pool = other.m_Pool;
	other.m_Pool = VK_NULL_HANDLE;

	return *this;
}
