#pragma once
#include <queue>
#include <optional>

class Mouse
{
	friend class Window;
public:
	struct RawDelta
	{
		int x, y;
	};
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			x(0),
			y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return{ x,y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool MiddleIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool LeftIsReleased() const noexcept;
	float GetWheelDelta() noexcept;
	Mouse::Event Read() noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
	float mouseWheelDelta = 0;
private:
	void OnRawDelta(int dx, int dy) noexcept;
	void OnWheelDelta(float delta);
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;

	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnMiddleUp(int x, int y) noexcept;
	void OnMiddleDown(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimRawInputBuffer() noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr uint32_t bufferSize = 16u;
	int x = 0;
	int y = 0;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool middleIsPressed = false;
	bool isInWindow = false;
	float wheelDelta = 0;
	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;
public:
	const int& c_x = x;
	const int& c_y = y;
};