#pragma once
#include <bitset>
#include <queue>

class Keyboard
{
	friend class Window;

public:
	class Event
	{
	public:
		enum class Type { Press, Release, Invalid };
		
	private:
		Type m_type;
		unsigned char m_code;

	public:
		Event() :
			m_type(Type::Invalid), m_code(0u)
		{}
		Event(Type type, unsigned char code) :
			m_type(type), m_code(code)
		{}
		bool IsPress() const { return m_type == Type::Press; }
		bool IsRelease() const { return m_type == Type::Release; }
		bool IsValid() const { return m_type != Type::Invalid; }
		unsigned char Code() const { return m_code; }
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	bool KeyIsPressed(unsigned char key) const { return m_keyStates[key]; }
	bool KeyPressedThisFrame(unsigned char key) const
	{
		return m_keyStates[key] && !m_prevKeyStates[key];
	}
	Event ReadKey();
	bool KeyIsEmpty() const { return m_keyBuffer.empty(); }

	char ReadChar();
	bool CharIsEmpty() const { return m_charBuffer.empty(); }

	void ClearKeyBuffer() { m_keyBuffer = std::queue<Event>(); }
	void ClearCharBuffer() { m_charBuffer = std::queue<char>(); }
	void ClearBuffers() { ClearKeyBuffer(); ClearCharBuffer(); }
	
	void SetAutoRepeat(bool value) { m_autoRepeat = value; }
	bool AutoRepeatIsEnabled() const { return m_autoRepeat; }

	void EndFrame() { m_prevKeyStates = m_keyStates; }

private:
	void OnKeyPressed(unsigned char key);
	void OnKeyReleased(unsigned char key);
	void OnChar(char c);
	void ClearState() { m_keyStates.reset(); }

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer);

private:
	static constexpr unsigned int N_KEYS = 256u;
	static constexpr unsigned int BUFFER_SIZE = 16u;

	bool m_autoRepeat = false;

	std::bitset<N_KEYS> m_keyStates;
	std::bitset<N_KEYS> m_prevKeyStates;
	std::queue<Event>	m_keyBuffer;
	std::queue<char>	m_charBuffer;
};

