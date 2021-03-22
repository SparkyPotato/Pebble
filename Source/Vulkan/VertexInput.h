#pragma once

#include "Instance.h"

struct VertexElement
{
	VkFormat Format;
	u32 Location;
};

class VertexInput
{
public:
	VertexInput() = default;
	VertexInput(VkPrimitiveTopology topology, const std::initializer_list<VertexElement>& elements);

	VertexInput(const VertexInput& other);
	VertexInput(VertexInput&& other);

	VertexInput& operator=(const VertexInput& other);
	VertexInput& operator=(VertexInput&& other);

	const VkPipelineVertexInputStateCreateInfo& GetInputInfo() const { return m_InputInfo; }
	const VkPipelineInputAssemblyStateCreateInfo& GetAssemblyInfo() const { return m_AssemblyInfo; }

private:
	// What is this formatting???
	VkPipelineVertexInputStateCreateInfo m_InputInfo{ .sType =
														  VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	VkPipelineInputAssemblyStateCreateInfo m_AssemblyInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
	};

	VkVertexInputBindingDescription m_Binding{};
	std::vector<VkVertexInputAttributeDescription> m_Attributes;
};
