#pragma once

#include "Instance.h"

class PipelineLayout;

class DescriptorSet
{
public:
	DescriptorSet() = default;
	~DescriptorSet();

	DescriptorSet(const DescriptorSet& other) = delete;
	DescriptorSet& operator=(const DescriptorSet& other) = delete;

	DescriptorSet(DescriptorSet&& other);
	DescriptorSet& operator=(DescriptorSet&& other);

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
