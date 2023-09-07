#pragma once
#include "Managers/AssetManager.h"
#include "Assets/SprAsset.h"

namespace workspace
{
	std::vector<std::unique_ptr<Frame>> m_assets;

	void Add(const std::string& path, uint32_t group, uint32_t frame, float x, float y)
	{
		auto* asset = AssetManager::Instance().Get(FileType::SPR, path);

		if (asset)
		{
			auto* spr = reinterpret_cast<SprAsset*>(asset);

			auto f = spr->GetFrame(group, frame);

			if (f)
			{
				printf("%f %f\n", x, y);

				std::unique_ptr<Frame> temp = std::make_unique<Frame>(*f);
				temp->x = x;
				temp->y = y;
				m_assets.emplace_back(std::move(temp));
			}
		}
	}
}