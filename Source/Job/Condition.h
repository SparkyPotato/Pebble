#pragma once

class WaitCondition
{
public:
	virtual ~WaitCondition() = default;

	virtual operator bool() const = 0;

	virtual void SleepOn() const = 0;
};
