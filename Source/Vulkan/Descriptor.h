#pragma once

#include "Instance.h"

class Buffer;
class PipelineLayout;

struct BufferUpdate
{
	const Buffer& Buffer;
	u64 Offset;
	u64 Range;
};

class DescriptorSet
{
public:
	DescriptorSet() = default;
	~DescriptorSet();

	DescriptorSet(const DescriptorSet& other) = delete;
	DescriptorSet& operator=(const DescriptorSet& other) = delete;

	DescriptorSet(DescriptorSet&& other);
	DescriptorSet& operator=(DescriptorSet&& other);

	void Update(u32 binding, u32 arrayElement, VkDescriptorType type, std::span<BufferUpdate> buffers);

	VkDescriptorSet GetHandle() const { return m_Set; }

private:
	friend class DescriptorPool;

	DescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout);

	VkDescriptorSet m_Set = VK_NULL_HANDLE;
	VkDescriptorPool m_Pool = VK_NULL_HANDLE;
};

class DescriptorPool
{
public:
	DescriptorPool() = default;
	DescriptorPool(std::span<VkDescriptorPoolSize> sizes, u32 maxSets);
	~DescriptorPool();

	DescriptorPool(const DescriptorPool& other) = delete;
	DescriptorPool& operator=(const DescriptorPool& other) = delete;

	DescriptorPool(DescriptorPool&& other);
	DescriptorPool& operator=(DescriptorPool&& other);

	DescriptorSet Allocate(const PipelineLayout& layout, u32 layoutIndex);

private:
	VkDescriptorPool m_Pool = VK_NULL_HANDLE;
};
