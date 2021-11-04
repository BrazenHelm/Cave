#include "Keyboard.h"

Keyboard::Event Keyboard::ReadKey()
{
	if (m_keyBuffer.size() > 0u)
	{
		Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}


char Keyboard::ReadChar()
{
	if (m_charBuffer.size() > 0u)
	{
		char c = m_charBuffer.front();
		m_charBuffer.pop();
		return c;
	}
	else
	{
		return 0;
	}
}


void Keyboard::OnKeyPressed(unsigned char key)
{
	m_keyStates[key] = true;
	m_keyBuffer.push(Event(Event::Type::Press, key));
	TrimBuffer(m_keyBuffer);
}


void Keyboard::OnKeyReleased(unsigned char key)
{
	m_keyStates[key] = false;
	m_keyBuffer.push(Event(Event::Type::Release, key));
	TrimBuffer(m_keyBuffer);
}


void Keyboard::OnChar(char c)
{
	m_charBuffer.push(c);
	TrimBuffer(m_charBuffer);
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
	while (buffer.size() > BUFFER_SIZE)
	{
		buffer.pop();
	}
}

