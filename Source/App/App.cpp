#include "App.h"

App::App()
{
	m_MainWindow = Window("Pebble", { 1600, 900 });
}

void App::Run()
{
	while (!m_MainWindow.ShouldClose())
	{
		
	}
}
