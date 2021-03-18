#pragma once

struct Job
{
	std::function<void(std::any&)> Func;

	std::any Argument;
};

static_assert(sizeof(Job) % 64 == 0, "Size of job must be a multiple of 64! Add Padding.");
