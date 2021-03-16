#include "PCH.h"

#include "Input.h"

#include "Window.h"

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto& input = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Input;

	switch (action)
	{
	case GLFW_PRESS:
		input.m_KeyStates[key] = true;
		if (input.m_KeyboardCaptured) { return; }
		for (auto& call : input.m_KeyCallbacks)
		{
			call(Key(key), true);
		}
		break;
	case GLFW_RELEASE:
		input.m_KeyStates[key] = false;
		if (input.m_KeyboardCaptured) { return; }
		for (auto& call : input.m_KeyCallbacks)
		{
			call(Key(key), false);
		}
		break;
	default: break;
	}
}

void Input::CharCallback(GLFWwindow* window, u32 codepoint)
{
	auto& input = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Input;

	if (input.m_KeyboardCaptured) { return; }
	for (auto& call : input.m_CharCallbacks)
	{
		call(codepoint);
	}
}

void Input::CursorCallback(GLFWwindow* window, double x, double y)
{
	auto& input = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Input;
	input.m_MouseState.Position.x = int32_t(x);
	input.m_MouseState.Position.y = int32_t(y);

	if (input.m_MouseCaptured) { return; }
	for (auto& call : input.m_MoveCallbacks)
	{
		call(input.m_MouseState.Position);
	}
}

void Input::ButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto& input = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Input;
	switch (action)
	{
	case GLFW_PRESS:
		input.m_MouseState.Buttons[button] = true;

		if (input.m_MouseCaptured) { return; }
		for (auto& call : input.m_ButtonCallbacks)
		{
			call(button, true);
		}
		break;
	case GLFW_RELEASE:
		input.m_MouseState.Buttons[button] = false;

		if (input.m_MouseCaptured) { return; }
		for (auto& call : input.m_ButtonCallbacks)
		{
			call(button, false);
		}
		break;
	default: break;
	}
}

void Input::ScrollCallback(GLFWwindow* window, double x, double y)
{
	auto& input = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Input;
	input.m_MouseState.WheelPos.x += float(x);
	input.m_MouseState.WheelPos.y += float(y);

	if (input.m_MouseCaptured) { return; }
	for (auto& call : input.m_ScrollCallbacks)
	{
		call({ float(x), float(y) });
	}
}

void Input::SetupCallbacks(GLFWwindow* window) 
{
	glfwSetKeyCallback(window, Input::KeyCallback);
	glfwSetCharCallback(window, Input::CharCallback);
	glfwSetCursorPosCallback(window, Input::CursorCallback);
	glfwSetMouseButtonCallback(window, Input::ButtonCallback);
	glfwSetScrollCallback(window, Input::ScrollCallback);
}

const char* Input::GetClipboardData()
{
	return glfwGetClipboardString(nullptr);
}

void Input::SetClipboardData(const char* data)
{
	glfwSetClipboardString(nullptr, data);
}
