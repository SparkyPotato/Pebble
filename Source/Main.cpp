#include "PCH.h"

#include "App/App.h"
#include "App/Logger.h"

int main(int argc, char* argv[])
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

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
