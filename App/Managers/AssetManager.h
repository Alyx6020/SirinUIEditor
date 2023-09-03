#pragma once
#include "Assets/IAsset.h"
#include <map>
#include <string>
#include "Graphics.h"
#include "Data.h"
#include <map>



class R3eAsset;
class FbxAsset;

class AssetManager
{
public:
	static AssetManager& Instance()
	{
		static AssetManager instance;

		return instance;
	}
private:
	AssetManager() {};

public:
	~AssetManager()
	{
	}
	IAsset* Get(FileType type, const std::string& str);
	bool Erase(FileType type, const std::string& str);
	int Register(FileType type, const std::string& name, BaseFile& assetInfo);
	void Add(const std::string& path);


	template <typename T>
	void Add(AssetKey key, std::shared_ptr<T> asset)
	{
		m_assets.insert_or_assign(key, std::move(asset));
	}

private:
	std::map<AssetKey, std::shared_ptr<IAsset>> m_assets{};
};