#include "PCH.h"

#include "JobSystem.h"

#include "Fiber.h"
#include "MPMCQueue.h"

namespace JobSystem {

class Counter : public WaitCondition
{
public:
	Counter() = default;
	Counter(u64 value) : Value(value) {}

	Counter(const Counter& other) : Value(other.Value.load()) {}

	operator bool() const override { return Value == 0; }

	void SleepOn() const override
	{
		while (Value != 0)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(10ms);
		}
	}

	std::atomic<u64> Value = 0;
};

struct Fiber
{
	FiberContext Context;
	u8 Stack[32 * 1024]; // 32 KB
};

struct JobContext
{
	Job* RunningJob = nullptr;
	FiberContext* Context = nullptr;
	Counter* OnComplete = nullptr;
};

struct SleepingJob
{
	JobContext Context;
	const WaitCondition* WaitingOn;
};

std::vector<std::thread> s_Threads;

std::vector<Fiber> s_Fibers;
MPMCQueue<u64> s_FreeFibers;
std::vector<Counter> s_Counters;
MPMCQueue<u64> s_FreeCounters;

MPMCQueue<JobContext> s_Jobs;
MPMCQueue<SleepingJob> s_SleepingJobs;

Counter s_MainJobCounter(1);

thread_local JobContext s_CurrentJobContext;
thread_local FiberContext s_ThreadContext;

void WorkerThread()
{
	while (!s_MainJobCounter)
	{
		JobContext job;
		bool runnableJob = s_Jobs.TryPop(job);
		if (runnableJob)
		{
			s_CurrentJobContext = job;
			SwapContext(&s_ThreadContext, s_CurrentJobContext.Context);
		}

		SleepingJob sleeping;
		bool sleepingRunnable = false;
		bool sleepingJob = false;
		do
		{
			sleepingJob = s_SleepingJobs.TryPop(sleeping);
			if (sleepingJob && *sleeping.WaitingOn)
			{
				s_Jobs.Push(sleeping.Context);
				sleepingRunnable = true;
			}
		} while (sleepingJob);

		if (!sleepingRunnable && !runnableJob) // Job queue is probably empty
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(2ms);
		}
	}
}

void FiberFunction()
{
	s_CurrentJobContext.RunningJob->Func(s_CurrentJobContext.RunningJob->Argument);

	s_CurrentJobContext.OnComplete->Value--;
	s_FreeFibers.Push(reinterpret_cast<Fiber*>(s_CurrentJobContext.Context) - s_Fibers.data());
	SetContext(&s_ThreadContext);
}

void Initialize(u16 threadCount, u64 memUsage)
{
	if (!threadCount)
	{
		threadCount = std::thread::hardware_concurrency() - 1;
	}

	u64 fiberCount = memUsage * 32;
	s_Fibers.resize(fiberCount);
	s_Counters.resize(fiberCount);
	s_FreeFibers = MPMCQueue<u64>(fiberCount);
	s_FreeCounters = MPMCQueue<u64>(fiberCount);
	s_Jobs = MPMCQueue<JobContext>(fiberCount);
	s_SleepingJobs = MPMCQueue<SleepingJob>(fiberCount);
	for (u64 i = 0; i < fiberCount; i++)
	{
		s_FreeFibers.Push(i);
		s_FreeCounters.Push(i);
	}

	s_Threads.reserve(threadCount);
	std::generate_n(std::back_inserter(s_Threads), threadCount, [] { return std::thread(&WorkerThread); });
}

void Quit()
{
	s_MainJobCounter.Value--;
	for (auto& thread : s_Threads)
	{
		thread.join();
	}
}

const WaitCondition& Submit(std::span<Job> jobs)
{
	Counter& counter = s_Counters[s_FreeCounters.Pop()];

	for (auto& job : jobs)
	{
		JobContext jobContext;
		jobContext.RunningJob = &job;
		Fiber& fiber = s_Fibers[s_FreeFibers.Pop()];
		jobContext.Context = &fiber.Context;
		jobContext.Context->rsp = fiber.Stack + sizeof(fiber.Stack);
		jobContext.Context->rip = &FiberFunction;
		jobContext.OnComplete = &counter;

		s_Jobs.Push(jobContext);
	}

	return counter;
}

void Wait(const WaitCondition& condition) 
{
	SleepingJob job;
	job.Context = s_CurrentJobContext;
	job.WaitingOn = &condition;

	s_SleepingJobs.Push(job);
	if (s_CurrentJobContext.Context) 
	{
		SwapContext(s_CurrentJobContext.Context, &s_ThreadContext);
	}
	else
	{
		while (!condition)
		{
			JobContext job;
			bool runnableJob = s_Jobs.TryPop(job);
			if (runnableJob)
			{
				s_CurrentJobContext = job;
				SwapContext(&s_ThreadContext, s_CurrentJobContext.Context);
			}

			SleepingJob sleeping;
			bool sleepingRunnable = false;
			bool sleepingJob = false;
			do
			{
				sleepingJob = s_SleepingJobs.TryPop(sleeping);
				if (sleepingJob && *sleeping.WaitingOn)
				{
					s_Jobs.Push(sleeping.Context);
					sleepingRunnable = true;
				}
			} while (sleepingJob);

			if (!sleepingRunnable && !runnableJob) // Job queue is probably empty
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(2ms);
			}
		}
	}
}

}
