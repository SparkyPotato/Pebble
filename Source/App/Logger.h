#pragma once

#include "spdlog/spdlog.h"

class Logger
{
public:
	Logger() = delete;

	static const std::shared_ptr<spdlog::logger>& Get();

private:
	static std::shared_ptr<spdlog::logger> s_Logger;
	static std::once_flag s_Init;
};

#define TRACE(...) Logger::Get()->trace(__VA_ARGS__)
#define DEBUG(...) Logger::Get()->debug(__VA_ARGS__)
#define INFO(...) Logger::Get()->info(__VA_ARGS__)
#define WARN(...) Logger::Get()->warn(__VA_ARGS__)
#define ERROR(...) Logger::Get()->error(__VA_ARGS__)
#define CRITICAL(...)                                                                                                  \
	do                                                                                                                 \
	{                                                                                                                  \
		Logger::Get()->critical(__VA_ARGS__);                                                                          \
		throw 1;                                                                                                       \
	} while (false)

#define ASSERT(condition, ...)                                                                                         \
	if (!(condition))                                                                                                  \
	{                                                                                                                  \
		CRITICAL(__VA_ARGS__);                                                                                         \
	}
