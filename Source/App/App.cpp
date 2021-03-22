#include "PCH.h"

#include "App.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

App::App() : m_MainFrameFence(VK_FENCE_CREATE_SIGNALED_BIT)
{
	m_MainWindow = Window("Pebble", { 1600, 900 });

	m_VertexBuffer = Buffer(sizeof(float) * 5 * 3, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);
	m_UniformBuffer = Buffer(sizeof(glm::mat4) * 3, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	Buffer staging(sizeof(float) * 5 * 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	auto data = reinterpret_cast<std::pair<glm::vec2, glm::vec3>*>(staging.Map());
	data[0] = { { 0.f, -0.5f }, { 1.f, 0.f, 0.f } };
	data[1] = { { 0.5f, 0.5f }, { 0.f, 1.f, 0.f } };
	data[2] = { { -0.5f, 0.5f }, { 0.f, 0.f, 1.f } };
	staging.Unmap();
	staging.Flush(0, VK_WHOLE_SIZE);

	auto buf = m_Pool.Allocate();
	buf.Begin();
	VkBufferCopy copy[] = { VkBufferCopy{ 0, 0, 60 } };
	buf.CopyBuffer(staging, m_VertexBuffer, copy);
	buf.End();
	CommandBuffer* bufs[] = { &buf };
	Fence fence;
	Instance::Submit(bufs, {}, {}, &fence);

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

	std::vector<DescriptorBinding> bindings = { { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
		VK_SHADER_STAGE_VERTEX_BIT } };
	m_Layout = PipelineLayout(std::span(&bindings, 1), {});

	m_Pipeline = Pipeline(shaders,
		VertexInput(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, { { VK_FORMAT_R32G32_SFLOAT, 0 }, { VK_FORMAT_R32G32B32_SFLOAT, 1 } }),
		m_MainViewport, Rasterizer(VK_FRONT_FACE_COUNTER_CLOCKWISE), Multisample(), DepthStencil(),
		BlendState{ { VkPipelineColorBlendAttachmentState{
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
							  | VK_COLOR_COMPONENT_A_BIT } } },
		DynamicState{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }, m_Layout, m_Pass, 0);

	auto generate = [this](u32 w, u32 h) {
		auto& views = m_MainWindow.GetSwapchain().GetViews();
		m_MainFramebuffers.reserve(views.size());
		m_MainFramebuffers.clear();
		m_MainBuffers.reserve(views.size());
		m_MainBuffers.clear();

		VkDescriptorPoolSize size{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = u32(views.size()) };
		m_DPool = DescriptorPool(std::span(&size, 1), u32(views.size()));

		for (const auto& view : views)
		{
			const ImageView* attachments[] = { &view };
			Framebuffer& framebuffer = m_MainFramebuffers.emplace_back(m_Pass, glm::u32vec2(w, h), 1, attachments);
			CommandBuffer& buffer = m_MainBuffers.emplace_back(m_Pool.Allocate());
			DescriptorSet& set = m_Descriptors.emplace_back(m_DPool.Allocate(m_Layout, 0));

			buffer.Begin();
			VkClearValue values[] = { VkClearColorValue{ 0.f, 0.f, 0.f, 1.f } };
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

	fence.WaitOn();
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

void App::UpdateUniformBuffer()
{
	static auto start = std::chrono::high_resolution_clock::now();

	auto now = std::chrono::high_resolution_clock::now();
	float dt = std::chrono::duration<float>(now - start).count();

	glm::mat4* data = reinterpret_cast<glm::mat4*>(m_UniformBuffer.Map());

	data[0] = glm::rotate(glm::mat4(1.f), dt * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	data[1] = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
	data[2] = glm::perspective(
		glm::radians(45.f), m_MainViewport.GetViewport().width / m_MainViewport.GetViewport().height, 0.1f, 10.f);
	data[2][1][1] *= -1.f;

	m_UniformBuffer.Unmap();
}
