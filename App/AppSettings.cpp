#include "AppSettings.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
#include "Settings.h"
#include <DirectXMath.h>
#include <boost/tokenizer.hpp>

namespace pt = boost::property_tree;

namespace settings
{
	std::filesystem::path file;

	namespace
	{
		pt::ptree tree;
	}

	DirectX::XMFLOAT3 LoadFromString(const std::string& str)
	{
		boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
		boost::tokenizer<boost::char_separator<char> > tok(str, sep);
		float vec[3];

		int i = 0;
		for (auto t = tok.begin(); t != tok.end(); ++t)
		{
			vec[i++] = std::stof(*t);
		}

		return { vec[0],vec[1],vec[2] };
	}

	std::string SaveFromVector(const DirectX::XMFLOAT3& vec)
	{
		return std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
	}

	void Load()
	{
		if (!std::filesystem::exists("settings.ini"))
		{
			tree.put("settings.spriteImagePath", "");
			pt::write_ini("settings.ini", tree);
		}

		pt::read_ini("settings.ini", tree);

		settings::spriteImagePath = tree.get("settings.spriteImagePath", "");
	}

	void Update()
	{
		tree.put("settings.spriteImagePath", settings::spriteImagePath.string());
		pt::write_ini("settings.ini", tree);
	}
}