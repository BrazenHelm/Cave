#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	float Reset();
	float Split();
	float Time() const;

private:
	std::chrono::steady_clock::time_point m_lastReset;
	std::chrono::steady_clock::time_point m_lastSplit;
};

