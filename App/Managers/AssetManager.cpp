#include "AssetManager.h"
#include "Data.h"
#include <Assets/SprAsset.h>

IAsset* AssetManager::Get(FileType type, const std::string& str)
{
	auto iter = m_assets.find({ type,str });

	if (iter != m_assets.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

bool AssetManager::Erase(FileType type, const std::string& str)
{
	auto iter = m_assets.find({ type, str });

	if (iter != m_assets.end())
	{
		return m_assets.erase({ type,str });
	}

	return false;
}

int AssetManager::Register(FileType type, const std::string& name, BaseFile& assetInfo)
{
	switch (type)
	{
	case FileType::SPR:
	{
		SprFile* temp = static_cast<SprFile*>(&assetInfo);

		temp->path = std::filesystem::path(temp->path).replace_extension("" + FileType::SPR).string();

		std::shared_ptr<SprAsset> asset = std::make_shared<SprAsset>(temp->name, temp->path, temp->data, temp);

		auto it = m_assets.insert_or_assign({ FileType::SPR,temp->path }, std::move(asset));

		break;
	}
	}
	return 0;
}
