#include "PCH.h"

#include "Window.h"

Window::Window(const char* title, glm::u32vec2 dimensions, Style style)
{
	switch (style)
	{
	case Style::Default:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		break;
	case Style::NoResize:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		break;
	case Style::NoBorder:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		break;
	case Style::Splash:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		break;
	}

	m_Window = glfwCreateWindow(dimensions.x, dimensions.y, title, nullptr, nullptr);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetWindowRefreshCallback(m_Window, &Window::WindowRefreshCallback);
	m_Input.SetupCallbacks(m_Window);

	TRACE("Create window '{}'", title);

	m_Swapchain = Swapchain(m_Window);
}

Window::~Window() 
{ 
	glfwDestroyWindow(m_Window); 
}

Window::Window(Window&& other) noexcept
{
	m_Window = other.m_Window;
	other.m_Window = nullptr;
	m_Input = std::move(other.m_Input);
	m_Swapchain = std::move(other.m_Swapchain);
	glfwSetWindowUserPointer(m_Window, this);
}

Window& Window::operator=(Window&& other) noexcept
{
	this->~Window();

	m_Window = other.m_Window;
	other.m_Window = nullptr;
	m_Input = std::move(other.m_Input);
	m_Swapchain = std::move(other.m_Swapchain);
	glfwSetWindowUserPointer(m_Window, this);

	return *this;
}

void Window::PollEvents() { glfwPollEvents(); }

void Window::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
}

void Window::Cleanup() { glfwTerminate(); }

bool Window::ShouldClose() { return glfwWindowShouldClose(m_Window); }

void Window::SetRedrawCallback(std::function<void()> callback) { m_RedrawCallback = callback; }

void Window::WindowRefreshCallback(GLFWwindow* window) 
{
	auto& owindow = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

	if (owindow.m_RedrawCallback)
	{
		owindow.m_RedrawCallback();
	}
}
