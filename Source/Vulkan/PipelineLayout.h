#pragma once

#include "Instance.h"

struct DescriptorBinding
{
	u32 Binding;
	VkDescriptorType Type;
	u32 Count;
	VkShaderStageFlags Stages;
};

struct PushRange
{
	u32 Size;
	VkShaderStageFlags Stage;
};

class PipelineLayout
{
public:
	PipelineLayout();
	PipelineLayout(std::span<std::vector<DescriptorBinding>> layouts, std::span<PushRange> pushRanges);
	~PipelineLayout();

	PipelineLayout(const PipelineLayout& other) = delete;
	PipelineLayout& operator=(const PipelineLayout& other) = delete;

	PipelineLayout(PipelineLayout&& other);
	PipelineLayout& operator=(PipelineLayout&& other);

	VkPipelineLayout GetHandle() const { return m_Layout; }

private:
	VkPipelineLayout m_Layout = VK_NULL_HANDLE;
};
