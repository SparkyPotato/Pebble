#include "PCH.h"

#include "App/App.h"
#include "App/Logger.h"

struct InstanceHandler
{
	InstanceHandler() { Instance::Init(); }
	~InstanceHandler() { Instance::Cleanup(); }
};

struct WindowHandler
{
	WindowHandler() { Window::Init(); }
	~WindowHandler() { Window::Cleanup(); }
};

int main(int argc, char* argv[])
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

	try
	{
		WindowHandler w;
		InstanceHandler i;

		App app;
		app.Run();

		spdlog::shutdown();

		return 0;
	}
	catch (std::exception& e)
	{
		ERROR("Unexpected exception: {}", e.what());
		spdlog::shutdown();
	}
	catch (int)
	{
		spdlog::shutdown();
	}
	catch (...)
	{
		ERROR("Unknown exception thrown");
		spdlog::shutdown();
	}

	return 1;
}
