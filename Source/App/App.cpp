#include "PCH.h"

#include "App.h"

#include "Job/JobSystem.h"

App::App()
{
	m_MainWindow = Window("Pebble", { 1600, 900 });
	JobSystem::Initialize();
}

App::~App() { JobSystem::Quit(); }

void App::Run()
{
	while (!m_MainWindow.ShouldClose())
	{
		Window::PollEvents();
	}
}
