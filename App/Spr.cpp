#include "Spr.h"
#include <utility>
#include <Data.h>
#include <imgui.h>
#include <Settings.h>
#include "Managers/FileOpenManager.h"
#include <Managers/AssetManager.h>

namespace spr
{
	namespace
	{
		SpriteViewMode mode = SpriteViewMode::Fullsize;
		bool showSingle = false;
	}

	ImVec2 ResizeImage(float w, float h, float tar)
	{
		float ratio = std::min(tar / w, tar / h);

		return { w * ratio, h * ratio };
	}

	std::vector<std::unique_ptr<SprViewerGui>> m_sprViewerGui;
	Directory m_root;
	bool m_loaded = false;

	void spr::SprViewerGui::Render(std::pair<int, int> windowSize)
	{
		if (pDestory)
			return;

		ImGui::SetNextWindowSize(ImVec2(220, 350), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(
			(windowSize.first * 0.5f) - (110.0f),
			(windowSize.second * 0.5f) - (175.0f))
			, ImGuiCond_Once
		);

		if (ImGui::Begin((m_sprPath.filename().string() + "###" + std::to_string(pUid)).c_str(), &pOpen, ImGuiWindowFlags_NoSavedSettings))
		{
			if (spr)
			{
				if (ImGui::Button("Fullsize"))
				{
					mode = SpriteViewMode::Fullsize;
				}
				ImGui::SameLine();
				if (ImGui::Button("Med"))
				{
					mode = SpriteViewMode::Medium;
				}
				ImGui::SameLine();
				if (ImGui::Button("Small"))
				{
					mode = SpriteViewMode::Small;
				}
				ImGui::SameLine();
				ImGui::Checkbox("Single Row", &showSingle);

				int group = 0;
				int frame = 0;

				ImGui::BeginChild((m_sprPath.filename().string() + "###c" + std::to_string(pUid)).c_str());

				ImGuiStyle& style = ImGui::GetStyle();
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

				int id = 0;

				for (auto& iter : spr->m_actionList)
				{
					frame = 0;
					
					for (auto& it : iter.frames)
					{
						ImGui::BeginGroup();
						ImGui::PushID(id);
						ImGui::Text("%d - %d (%s)", group, frame, it.format);
						
						ImVec2 cursorBegin = ImGui::GetCursorPos();
						ImVec2 imgSize = mode == SpriteViewMode::Fullsize ? ImVec2(it.xwidth, it.xheight) :
							ResizeImage(it.xwidth, it.xheight, static_cast<uint32_t>(mode));
						
						ImGui::Image((void*)it.srv.Get(), imgSize);
						ImGui::EndGroup();

						ImVec2 cursorEnd = ImGui::GetCursorPos();
						ImGui::SetCursorPos(cursorBegin);

						ImGui::InvisibleButton("drag area", ImVec2(imgSize.x, imgSize.y));

						if (ImGui::BeginDragDropSource())
						{
							SprPayload temp(spr->path.string(), it.group, it.frame);
							ImGui::SetDragDropPayload("_RFS", &temp, sizeof(SprPayload));


							ImGui::Text("%d - %d", it.group, it.frame);
							ImGui::Image((void*)it.srv.Get(), imgSize);
							ImGui::EndDragDropSource();
						}

						//float last_button_x2 = ImGui::GetItemRectMax().x;
						//float next_button_x2 = last_button_x2 + style.ItemSpacing.x + 200;

						/*if (showSingle) {}
						else
						{
							if (frame + 1 < iter.frameCount && next_button_x2 < window_visible_x2)
								ImGui::SameLine();
						}*/
						ImGui::PopID();

						id++;
						frame++;
					}
					group++;
				}

				ImGui::EndChild();
			}

		}
		ImGui::End();

		if (!pOpen)
		{
			pDestory = true;
		}
	}

	void AddDirectoryNodes(Directory& parentNode, std::filesystem::directory_iterator directoryIterator)
	{
		for (const std::filesystem::directory_entry& entry : directoryIterator)
		{
			if (!entry.is_directory() && entry.path().extension() != "" + FileType::SPR) continue;
			Directory& childNode = parentNode.Children.emplace_back();
			childNode.path = entry.path();
			childNode.filename = entry.path().filename().u8string();
			if (childNode.IsDirectory = entry.is_directory(); childNode.IsDirectory)
			{
				AddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
			}
		}

		auto moveDirectoriesToFront = [](const Directory& a, const Directory& b) { return (a.IsDirectory > b.IsDirectory); };
		std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
	}

	Directory CreateDirectryFromPath(const std::filesystem::path& rootPath)
	{
		Directory rootNode;
		rootNode.path = rootPath;
		rootNode.filename = rootPath.filename().u8string();
		if (rootNode.IsDirectory = std::filesystem::is_directory(rootPath); rootNode.IsDirectory)
			AddDirectoryNodes(rootNode, std::filesystem::directory_iterator(rootPath));

		return rootNode;
	}

	void DisplayDirectory(const Directory& parentNode)
	{
		ImGui::PushID(&parentNode);
		if (parentNode.IsDirectory)
		{
			if (&m_root == &parentNode)
			{
				ImGui::BeginChild(("B" + parentNode.filename).c_str(), { 0,0 }, true);
				for (const Directory& childNode : parentNode.Children)
					DisplayDirectory(childNode);
				ImGui::EndChild();
			}
			else
			{
				// Non ROOT
				if (parentNode.Children.empty())
				{

				}
				else
				{
					if (ImGui::CollapsingHeader(parentNode.filename.c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
					{
						ImGui::BeginChild(("B" + parentNode.filename).c_str(), { 0,0 }, true);
						for (const Directory& childNode : parentNode.Children)
							DisplayDirectory(childNode);
						ImGui::EndChild();

					}

				}

			}


		}
		else
		{
			auto leaf = ImGui::CollapsingHeader(parentNode.filename.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth);

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				// Open File
				IAsset* asset = nullptr;
				asset = AssetManager::Instance().Get(FileType::SPR, parentNode.path.string());
				
				if (!asset)
				{
					FileOpenManager::Get().Open(parentNode.path);
					asset = AssetManager::Instance().Get(FileType::SPR, parentNode.path.string());
				}
				
				// Open new window
				m_sprViewerGui.emplace_back(std::make_unique<SprViewerGui>(parentNode.path, reinterpret_cast<SprAsset*>(asset)));

			}
		}
		ImGui::PopID();
	}

	void Load()
	{
		auto path = settings::GetSpritePath();
		m_root = CreateDirectryFromPath(path);
		m_loaded = true;
	}

	void Render()
	{
		if (!m_loaded) return;
		DisplayDirectory(m_root);

		for (auto& i : m_sprViewerGui)
		{
			i->Render({ 400, 400 });
		}

		// Remove old windows
		m_sprViewerGui.erase(
			std::remove_if(
				m_sprViewerGui.begin(),
				m_sprViewerGui.end(),
				[](std::unique_ptr<SprViewerGui> const& p) 
				{
					if (p)
					{
						return p->pDestory; 
					}
					else
					{
						return false;
					}
				}
			),
			m_sprViewerGui.end()
		);
	}

}