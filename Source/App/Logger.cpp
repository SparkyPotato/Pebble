#include "PCH.h"

#include "Logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::s_Logger;
std::once_flag Logger::s_Init;

const std::shared_ptr<spdlog::logger>& Logger::Get()
{
	std::call_once(s_Init, []() {
		auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifdef NDEBUG
		console->set_level(spdlog::level::info);
#else
		console->set_level(spdlog::level::debug);
#endif
		console->set_pattern("[%I:%M:%S.%e] %^%l%$: %v");

		auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Log.txt", true);
		file->set_level(spdlog::level::trace);
		file->set_pattern("[%I:%M:%S.%e] %^%l%$: %v");

		s_Logger = std::make_shared<spdlog::logger>(
			"Main", std::initializer_list<std::shared_ptr<spdlog::sinks::sink>>{ console, file });
		s_Logger->set_level(spdlog::level::trace);

		spdlog::flush_every(std::chrono::seconds(5));
	});

	return s_Logger;
}
