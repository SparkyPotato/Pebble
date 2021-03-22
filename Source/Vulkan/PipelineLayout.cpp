#include "PCH.h"

#include "PipelineLayout.h"

PipelineLayout::PipelineLayout()
{
	VkPipelineLayoutCreateInfo info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr };

	VkCall(vkCreatePipelineLayout(Instance::Device(), &info, nullptr, &m_Layout));
}

PipelineLayout::PipelineLayout(std::span<std::vector<DescriptorBinding>> layouts, std::span<PushRange> pushRanges)
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	m_DescriptorLayouts.reserve(layouts.size());
	for (const auto& layout : layouts)
	{
		bindings.reserve(layout.size());

		for (const auto& binding : layout)
		{
			bindings.push_back(VkDescriptorSetLayoutBinding{ .binding = binding.Binding,
				.descriptorType = binding.Type,
				.descriptorCount = binding.Count,
				.stageFlags = binding.Stages });
		}

		VkDescriptorSetLayoutCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = u32(bindings.size()),
			.pBindings = bindings.data() };

		VkDescriptorSetLayout descLayout;
		VkCall(vkCreateDescriptorSetLayout(Instance::Device(), &info, nullptr, &descLayout));
		m_DescriptorLayouts.push_back(descLayout);

		bindings.clear();
	}

	std::vector<VkPushConstantRange> ranges;
	ranges.reserve(pushRanges.size());
	for (u32 offset = 0; const auto& range : pushRanges)
	{
		ranges.push_back(VkPushConstantRange{ .stageFlags = range.Stage, .offset = offset, .size = range.Size });

		offset += range.Size;
	}

	VkPipelineLayoutCreateInfo info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = u32(m_DescriptorLayouts.size()),
		.pSetLayouts = m_DescriptorLayouts.data(),
		.pushConstantRangeCount = u32(ranges.size()),
		.pPushConstantRanges = ranges.data() };

	VkCall(vkCreatePipelineLayout(Instance::Device(), &info, nullptr, &m_Layout));
}

PipelineLayout::~PipelineLayout()
{
	vkDestroyPipelineLayout(Instance::Device(), m_Layout, nullptr);

	for (VkDescriptorSetLayout layout : m_DescriptorLayouts)
	{
		vkDestroyDescriptorSetLayout(Instance::Device(), layout, nullptr);
	}
}

PipelineLayout::PipelineLayout(PipelineLayout&& other)
{
	m_Layout = other.m_Layout;
	other.m_Layout = VK_NULL_HANDLE;
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other)
{
	this->~PipelineLayout();

	m_Layout = other.m_Layout;
	other.m_Layout = VK_NULL_HANDLE;

	return *this;
}
