#pragma once

#include "Window/Window.h"

class App
{
public:
	App();
	~App();

	void Run();

private:
	Window m_MainWindow;
};
