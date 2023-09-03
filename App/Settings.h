#pragma once
#include <filesystem>

#define CONSOLE_DEBUG
#define DRAW_ALL_LEAFS

#ifdef CONSOLE_DEBUG
#include <iostream>
#endif

namespace DirectX
{
	struct XMFLOAT4;
	struct XMFLOAT3;
}

namespace settings
{
	extern std::filesystem::path spriteImagePath;

	const std::string GetSpritePath();
}