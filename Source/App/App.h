#pragma once

#include "Window/Window.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/Command.h"
#include "Vulkan/Descriptor.h"
#include "Vulkan/Framebuffer.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Sync.h"

class App
{
public:
	App();
	~App();

	void Run();

private:
	void UpdateUniformBuffer();

	Window m_MainWindow;
	std::vector<Framebuffer> m_MainFramebuffers;

	Buffer m_VertexBuffer;
	Buffer m_UniformBuffer;
	Pipeline m_Pipeline;
	PipelineLayout m_Layout;
	RenderPass m_Pass;
	Viewport m_MainViewport;

	CommandPool m_Pool;
	std::vector<CommandBuffer> m_MainBuffers;

	DescriptorPool m_DPool;
	std::vector<DescriptorSet> m_Descriptors;

	Semaphore m_MainImageAvailable;
	Semaphore m_MainRenderFinished;
	Fence m_MainFrameFence;
	std::function<void()> m_Draw;
};
