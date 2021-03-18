#pragma once

#include "Condition.h"
#include "Job.h"

namespace JobSystem {

void Initialize(u16 threadCount = 0, u64 memUsage = 100);

void Quit();

const WaitCondition& Submit(std::span<Job> jobs);

void Wait(const WaitCondition& condition);

}
