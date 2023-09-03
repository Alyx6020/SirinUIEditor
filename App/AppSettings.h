#pragma once
#include <filesystem>

namespace settings
{
	extern std::filesystem::path file;

	void Load();
	void Update();
}