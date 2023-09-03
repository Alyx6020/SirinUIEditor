#include "Settings.h"
#include <DirectXMath.h>


namespace settings
{

	
	std::filesystem::path spriteImagePath;

	const std::string GetSpritePath()
	{
		return spriteImagePath.string();
	}

}