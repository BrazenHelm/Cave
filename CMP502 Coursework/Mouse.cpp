#include "Mouse.h"



Mouse::Event Mouse::Read()
{
	if (m_buffer.size() > 0u)
	{
		Event e = m_buffer.front();
		m_buffer.pop();
		return e;
	}
	else
	{
		return Event();
	}
}


std::optional<Mouse::RawDelta> Mouse::ReadRawDelta()
{
	if (m_rawDeltaBuffer.empty()) return std::nullopt;

	const RawDelta d = m_rawDeltaBuffer.front();
	m_rawDeltaBuffer.pop();
	return d;
}


void Mouse::OnRawDelta(int dx, int dy)
{
	m_rawDeltaBuffer.push(RawDelta{ dx, dy });
}


void Mouse::OnMouseMove(int x, int y)
{
	m_x = x; m_y = y;
	m_buffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}


void Mouse::OnLeftPressed(int x, int y)
{
	m_leftIsPressed = true;
	m_buffer.push(Event(Event::Type::LPress, *this));
	TrimBuffer();
}


void Mouse::OnLeftReleased(int x, int y)
{
	m_leftIsPressed = false;
	m_buffer.push(Event(Event::Type::LRelease, *this));
	TrimBuffer();
}


void Mouse::OnRightPressed(int x, int y)
{
	m_rightIsPressed = true;
	m_buffer.push(Event(Event::Type::RPress, *this));
	TrimBuffer();
}


void Mouse::OnRightReleased(int x, int y)
{
	m_rightIsPressed = false;
	m_buffer.push(Event(Event::Type::RRelease, *this));
	TrimBuffer();
}


void Mouse::OnWheelUp(int x, int y)
{
	m_buffer.push(Event(Event::Type::WheelUp, *this));
	TrimBuffer();
}


void Mouse::OnWheelDown(int x, int y)
{
	m_buffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}


void Mouse::TrimBuffer()
{
	while (m_buffer.size() > BUFFER_SIZE)
	{
		m_buffer.pop();
	}
}


void Mouse::EndFrame()
{
	m_leftWasPressed = m_leftIsPressed;
	m_rightWasPressed = m_rightWasPressed;
}

