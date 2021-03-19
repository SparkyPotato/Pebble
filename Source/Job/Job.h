#pragma once

struct Job
{
	std::function<void(std::any&)> Func;

	std::any Argument;
};
