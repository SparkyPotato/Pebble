#pragma once

#include <immintrin.h>

#ifdef _WIN32

struct FiberContext
{
	void (*rip)() = nullptr;
	void* rsp = nullptr;
	u64 rbx = 0, rbp = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0, rdi = 0, rsi = 0;
	__m128i xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
};

#else

struct FiberContext
{
	void (*rip)() = nullptr;
	void* rsp = nullptr;
	u64 rbx = 0, rbp = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0;
};

#endif

extern void (*SwapContext)(FiberContext* from, FiberContext* to);
extern void (*SetContext)(FiberContext* to);
