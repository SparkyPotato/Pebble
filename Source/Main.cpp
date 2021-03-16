#include "PCH.h"

#include "App/App.h"
#include "App/Logger.h"

int main()
{
	int returnVal = 0;
	try
	{
		Window::Init();
		Instance::Init();

		App app;
		app.Run();
	}
	catch (...)
	{
		returnVal = 1;
	}

	Instance::Cleanup();
	Window::Cleanup();
	spdlog::shutdown();
	return returnVal;
}
