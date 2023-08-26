#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Down,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event():type(Type::Invalid),code(0u) {}
		Event(Type type, unsigned char code) noexcept :
			type(type), code(code) {}

		bool IsDown() const
		{
			return type == Type::Down;
		}
		bool IsRelease() const
		{
			return type == Type::Release;
		}
		bool IsValid() const
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const
		{
			return code;
		}
	};

public:
	Keyboard() = default;

	bool KeyIsPressed(unsigned char keycode) const;
	std::optional<Event> ReadKey();
	bool KeyIsEmpty() const;
	void ClearKey();

	char ReadChar();
	bool CharIsEmpty() const;
	void ClearChar();
	void Clear();

	void EnableAutoRepeat();
	void DisableAutoRepeat();
	bool AutoRepeatIsEnabled();
private:
	void OnKeyPressed(unsigned char keycode);
	void OnKeyReleased(unsigned char keycode);
	void OnChar(char character);
	void ClearState();
	template<typename T>
	void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};


