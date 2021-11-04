#include "Timer.h"

using namespace std::chrono;


Timer::Timer()
{
	m_lastReset = steady_clock::now();
}


float Timer::Reset()
{
	const auto old = m_lastReset;
	m_lastReset = steady_clock::now();
	const duration<float> timeSinceReset = m_lastReset - old;
	return timeSinceReset.count();
}


float Timer::Split()
{
	const auto old = m_lastSplit;
	m_lastSplit = steady_clock::now();
	const duration<float> timeSinceSplit = m_lastSplit - old;
	return timeSinceSplit.count();
}


float Timer::Time() const
{
	return duration<float>(steady_clock::now() - m_lastReset).count();
}

