#pragma once
#include "FileOpenDialog.h"
#include <filesystem>

#include <thread>
#include <future>
#include <Graphics.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "ImGui/imgui_canvas.h"



class Gui
{
public:
	Gui(Graphics& gfx)
		:
		gfx(gfx)
	{
		graphics::gfx = &gfx;
	}

	ImVec2 m_pos;
	float m_zoom = 1.0f;

	Graphics& gfx;
    void RenderGui(void) noexcept;
private:
	void SaveTexturesToBitmap();
	void SaveTexturesToDDS();
	int OutputButtons();
	bool pDemoWindowOpen = false;

	bool outputButtonsDisabled = false;
	std::future<int> future;

	std::filesystem::path fileInput {};
	std::filesystem::path folderOutput {};


	ImVec2 m_workspaceSize = { 250, 250 };


	FileOpenDialog fileDialog;
};