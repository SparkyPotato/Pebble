#include "PCH.h"

#include "App.h"

App::App() : m_MainFrameFence(VK_FENCE_CREATE_SIGNALED_BIT)
{
	m_MainWindow = Window("Pebble", { 1600, 900 });

	m_VertexBuffer = Buffer(sizeof(float) * 5 * 3, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	auto data = reinterpret_cast<std::pair<glm::vec2, glm::vec3>*>(m_VertexBuffer.Map());
	data[0] = { { 0.f, -0.5f }, { 1.f, 0.f, 0.f } };
	data[1] = { { 0.5f, 0.5f }, { 0.f, 1.f, 0.f } };
	data[2] = { { -0.5f, 0.5f }, { 0.f, 0.f, 1.f } };
	m_VertexBuffer.Unmap();
	m_VertexBuffer.Flush(0, VK_WHOLE_SIZE);

	Shader shaders[] = { Shader("../Shaders/Triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
		Shader("../Shaders/Triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT) };

	VkAttachmentDescription attachments[] = { VkAttachmentDescription{
		.format = m_MainWindow.GetSwapchain().GetFormat(),
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR } };
	VkAttachmentReference refs[] = { VkAttachmentReference{
		.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	Subpass subpasses[] = { Subpass{ .BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS, .Color = refs } };
	VkSubpassDependency dependencies[] = { VkSubpassDependency{ .srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT } };
	m_Pass = RenderPass(attachments, subpasses, dependencies);

	m_MainViewport = Viewport{ { 0.f, 0.f }, { 1600.f, 900.f }, { 0.f, 1.f }, VkRect2D{ { 0, 0 }, { 1600, 900 } } };

	m_Pipeline = Pipeline(shaders,
		VertexInput(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, { { VK_FORMAT_R32G32_SFLOAT, 0 }, { VK_FORMAT_R32G32B32_SFLOAT, 1 } }),
		m_MainViewport, Rasterizer(VK_FRONT_FACE_CLOCKWISE), Multisample(), DepthStencil(),
		BlendState{ { VkPipelineColorBlendAttachmentState{
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
							  | VK_COLOR_COMPONENT_A_BIT } } },
		DynamicState{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }, PipelineLayout(), m_Pass, 0);

	auto generate = [this](u32 w, u32 h) {
		auto& views = m_MainWindow.GetSwapchain().GetViews();
		m_MainFramebuffers.reserve(views.size());
		m_MainFramebuffers.clear();
		m_MainBuffers.reserve(views.size());
		m_MainBuffers.clear();

		for (const auto& view : views)
		{
			const ImageView* attachments[] = { &view };
			Framebuffer& framebuffer = m_MainFramebuffers.emplace_back(m_Pass, glm::u32vec2(w, h), 1, attachments);
			CommandBuffer& buffer = m_MainBuffers.emplace_back(m_Pool.Allocate());

			buffer.Begin();
			VkClearValue values[] = { VkClearColorValue{ 0.f, 1.f, 0.f, 1.f } };
			buffer.BeginRenderPass(m_Pass, framebuffer, VkRect2D{ { 0, 0 }, { w, h } }, values);

			buffer.BindViewport(m_MainViewport);
			buffer.BindPipeline(m_Pipeline);
			buffer.BindVertexBuffer(m_VertexBuffer, 0);
			buffer.Draw(3, 1, 0, 0);

			buffer.EndRenderPass();
			buffer.End();
		}
	};
	generate(1600, 900);

	m_MainWindow.GetSwapchain().SetPreResizeCallback([this](u32 w, u32 h) {
		m_MainFrameFence.WaitOn();
		m_MainViewport = Viewport{ { 0.f, 0.f }, { float(w), float(h) }, { 0.f, 1.f }, VkRect2D{ { 0, 0 }, { w, h } } };
	});

	m_MainWindow.GetSwapchain().SetPostResizeCallback([this, generate](u32 w, u32 h) {
		generate(w, h);
		m_Draw();
	});

	m_Draw = [this]() {
		std::optional<u32> imageOpt = m_MainWindow.GetSwapchain().GetNextImage(&m_MainImageAvailable, nullptr);
		if (imageOpt)
		{
			m_MainFrameFence.WaitOn();
			m_MainFrameFence.Reset();
			u32 image = imageOpt.value();

			CommandBuffer* buffers[] = { &m_MainBuffers[image] };
			std::pair<const Semaphore*, VkPipelineStageFlags> wait[] = { { &m_MainImageAvailable,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } };
			const Semaphore* signal[] = { &m_MainRenderFinished };
			Instance::Submit(buffers, wait, signal, &m_MainFrameFence);

			const Swapchain* swapchains[] = { &m_MainWindow.GetSwapchain() };
			const Semaphore* swait[] = { &m_MainRenderFinished };
			u32 indices[] = { image };
			Swapchain::Present(swapchains, swait, indices);
		}
	};
	m_MainWindow.SetRedrawCallback(m_Draw);
}

App::~App() { Instance::WaitForIdle(); }

void App::Run()
{
	while (!m_MainWindow.ShouldClose())
	{
		Window::PollEvents();

		m_Draw();
	}
}
