#pragma once
#include <bitset>
#include <optional>
#include <queue>
#include <utility>


class Mouse
{
	friend class Window;

public:
	struct RawDelta { int dx, dy; };

	class Event
	{
	public:
		enum class Type
		{
			LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Invalid
		};

	private:
		Type m_type;
		bool m_leftIsPressed;
		bool m_rightIsPressed;
		int m_x, m_y;

	public:
		Event() : m_type(Type::Invalid), m_leftIsPressed(false),
			m_rightIsPressed(false), m_x(0), m_y(0)
		{};

		Event(Type type, const Mouse& parent) : m_type(type),
			m_leftIsPressed(parent.m_leftIsPressed),
			m_rightIsPressed(parent.m_rightIsPressed),
			m_x(parent.m_x), m_y(parent.m_y)
		{};

		bool IsValid() const { return m_type != Type::Invalid; }
		Type GetType() const { return m_type; }
		std::pair<int, int> GetPos() const { return { m_x, m_y }; }
		int GetPosX() const { return m_x; }
		int GetPosY() const { return m_y; }
		bool LeftIsPressed() const { return m_leftIsPressed; }
		bool RightIsPressed() const { return m_rightIsPressed; }
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPos() const { return { m_x, m_y }; }
	int GetPosX() const { return m_x; }
	int GetPosY() const { return m_y; }
	bool LeftIsPressed() const { return m_leftIsPressed; }
	bool RightIsPressed() const { return m_rightIsPressed; }
	bool LeftPressedThisFrame() const { return m_leftIsPressed && !m_leftWasPressed; }
	bool RightPressedThisFrame() const { return m_rightIsPressed && !m_rightWasPressed; }

	Mouse::Event Read();
	std::optional<Mouse::RawDelta> ReadRawDelta();
	bool IsEmpty() const { return m_buffer.empty(); }
	void Flush() { m_buffer = std::queue<Event>(); }

	void EndFrame();

private:
	void OnRawDelta(int dx, int dy);
	void OnMouseMove(int x, int y);
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void TrimBuffer();

private:
	static constexpr unsigned int BUFFER_SIZE = 16u;
	int m_x, m_y;
	bool m_leftIsPressed, m_rightIsPressed;
	bool m_leftWasPressed, m_rightWasPressed;
	std::queue<Event> m_buffer;
	std::queue<RawDelta> m_rawDeltaBuffer;
};

