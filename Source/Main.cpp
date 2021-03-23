#include "PCH.h"

#include "App/App.h"
#include "App/Logger.h"

int main(int argc, char* argv[])
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

	try
	{
		Window::Init();
		Instance::Init();

		App app;
		app.Run();

		Instance::Cleanup();
		Window::Cleanup();
		spdlog::shutdown();

		return 0;
	}
	catch (std::exception& e)
	{
		ERROR("Unexpected exception: {}", e.what());
	}
	catch (int)
	{
	}
	catch (...)
	{
		ERROR("Unknown exception thrown");
	}

	return 1;
}
