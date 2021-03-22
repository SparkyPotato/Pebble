#pragma once

#include "Window/Window.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/Command.h"
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
	Window m_MainWindow;
	std::vector<Framebuffer> m_MainFramebuffers;

	Buffer m_VertexBuffer;
	Pipeline m_Pipeline;
	RenderPass m_Pass;
	Viewport m_MainViewport;

	CommandPool m_Pool;
	std::vector<CommandBuffer> m_MainBuffers;

	Semaphore m_MainImageAvailable;
	Semaphore m_MainRenderFinished;
	Fence m_MainFrameFence;
	std::function<void()> m_Draw;
};
