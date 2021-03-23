#include "PCH.h"

#include "Descriptor.h"

#include "Buffer.h"
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

void DescriptorSet::Update(
	u32 binding, u32 arrayElement, VkDescriptorType type, std::span<BufferUpdate> buffers)
{
	static thread_local std::vector<VkDescriptorBufferInfo> bInfos;
	bInfos.clear();
	bInfos.reserve(buffers.size());
	for (const auto& buf : buffers)
	{
		bInfos.push_back(VkDescriptorBufferInfo{
			.buffer = buf.Buffer.GetHandle(),
			.offset = buf.Offset,
			.range = buf.Range
		});
	}

	VkWriteDescriptorSet info{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = m_Set,
		.dstBinding = binding,
		.dstArrayElement = arrayElement,
		.descriptorCount = u32(bInfos.size()),
		.descriptorType = type,
		.pBufferInfo = bInfos.data()
	};
	vkUpdateDescriptorSets(Instance::Device(), 1, &info, 0, nullptr);
}

DescriptorPool::DescriptorPool(std::span<VkDescriptorPoolSize> sizes, u32 maxSets)
{
	VkDescriptorPoolCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.poolSizeCount = u32(sizes.size()),
		.pPoolSizes = sizes.data(),
		.maxSets = maxSets };

	VkCall(vkCreateDescriptorPool(Instance::Device(), &info, nullptr, &m_Pool));
}

DescriptorSet DescriptorPool::Allocate(const PipelineLayout& layout, u32 layoutIndex)
{
	return DescriptorSet(m_Pool, layout.GetSetLayout(layoutIndex));
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(Instance::Device(), m_Pool, nullptr); }

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
