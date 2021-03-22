#pragma once

#include "Input.h"
#include "Vulkan/Swapchain.h"

class Window
{
public:
	enum class Style
	{
		Default, NoResize, NoBorder, Splash
	};

	Window() = default;
	Window(const char* title, glm::u32vec2 dimensions, Style style = Style::Default);
	~Window();

	Window(const Window& other) = delete;
	Window& operator=(const Window& other) = delete;

	Window(Window&& other) noexcept;
	Window& operator=(Window&& other) noexcept;

	static void PollEvents();

	static void Init();
	static void Cleanup();

	bool ShouldClose();

	const Input& GetInput() { return m_Input; }
	Swapchain& GetSwapchain() { return m_Swapchain; }

	void SetRedrawCallback(std::function<void()> callback);

private:
	friend class Input;
	friend class Swapchain;

	static void WindowRefreshCallback(GLFWwindow* window);

	GLFWwindow* m_Window = nullptr;
	Input m_Input;
	Swapchain m_Swapchain;
	std::function<void()> m_RedrawCallback;
};
