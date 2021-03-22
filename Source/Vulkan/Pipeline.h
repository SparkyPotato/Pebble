#pragma once

#include "Instance.h"

#include "Description.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "Shader.h"
#include "VertexInput.h"

class Pipeline
{
public:
	Pipeline() = default;
	Pipeline(std::span<Shader> shaders, const VertexInput& vertexInput, const Viewport& viewport,
		const Rasterizer& rasterizer, const Multisample& multisample, const DepthStencil& depthStencil,
		const BlendState& blendState, const DynamicState& dynamicState, const PipelineLayout& layout,
		const RenderPass& renderPass, u32 subpass);
	~Pipeline();

	Pipeline(const Pipeline& other) = delete;
	Pipeline& operator=(const Pipeline& other) = delete;

	Pipeline(Pipeline&& other);
	Pipeline& operator=(Pipeline&& other);

	VkPipeline GetHandle() const { return m_Pipeline; }

private:
	VkPipeline m_Pipeline = VK_NULL_HANDLE;
};
