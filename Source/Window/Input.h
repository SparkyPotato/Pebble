#pragma once

#include "GLFW/glfw3.h"

#include "Key.h"

struct MouseState
{
	glm::i32vec2 Position = { 0, 0 };
	std::bitset<8> Buttons;
	glm::vec2 WheelPos = { 0.f, 0.f };
};

class Input
{
public:
	void SetupCallbacks(GLFWwindow* window);

	static const char* GetClipboardData();
	static void SetClipboardData(const char* data);

	const MouseState& GetMouseState() const { return m_MouseState; }
	bool IsKeyDown(Key key) const { return m_KeyStates[u32(key)]; }

	template<typename Callable>
	void AddKeyEvent(const Callable& function)
	{
		m_KeyCallbacks.push_back(function);
	}

	template<typename Callable>
	void AddCharEvent(const Callable& function)
	{
		m_CharCallbacks.push_back(function);
	}

	template<typename Callable>
	void AddMouseButtonEvent(const Callable& function)
	{
		m_ButtonCallbacks.push_back(function);
	}

	template<typename Callable>
	void AddMouseMoveEvent(const Callable& function)
	{
		m_MoveCallbacks.push_back(function);
	}

	template<typename Callable>
	void AddMouseScrollEvent(const Callable& function)
	{
		m_ScrollCallbacks.push_back(function);
	}

	void CaptureMouse(bool capture) { m_MouseCaptured = capture; }
	void CaptureKeyboard(bool capture) { m_KeyboardCaptured = capture; }

private:
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CharCallback(GLFWwindow* window, u32 codepoint);
	static void CursorCallback(GLFWwindow* window, double x, double y);
	static void ButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void ScrollCallback(GLFWwindow* window, double x, double y);

	std::bitset<GLFW_KEY_LAST + 1> m_KeyStates;
	MouseState m_MouseState;

	std::vector<std::function<void(Key, bool)>> m_KeyCallbacks;
	std::vector<std::function<void(u32)>> m_CharCallbacks;
	std::vector<std::function<void(i8, bool)>> m_ButtonCallbacks;
	std::vector<std::function<void(glm::i32vec2)>> m_MoveCallbacks;
	std::vector<std::function<void(glm::vec2)>> m_ScrollCallbacks;

	bool m_KeyboardCaptured = false;
	bool m_MouseCaptured = false;
};
