#include "Keyboard.h"


bool Keyboard::KeyIsPressed(unsigned char keycode) const
{
	return keyStates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey()
{
	if (keyBuffer.size() > 0u)
	{
		Keyboard::Event e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}
	else
	{
		return std::optional<Event>();
	}
}


bool Keyboard::KeyIsEmpty() const
{
	return keyBuffer.empty();
}

char Keyboard::ReadChar()
{
	if (charBuffer.size() > 0u)
	{
		unsigned char charcode = charBuffer.front();
		charBuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const
{
	return charBuffer.empty();
}

void Keyboard::ClearKey()
{
	keyBuffer = std::queue<Event>();
}

void Keyboard::ClearChar()
{
	charBuffer = std::queue<char>();
}

void Keyboard::Clear()
{
	ClearKey();
	ClearChar();
}

void Keyboard::EnableAutoRepeat()
{
	autoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat()
{
	autoRepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled()
{
	return autoRepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode)
{
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Down, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode)
{
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character)
{
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState()
{
	keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
