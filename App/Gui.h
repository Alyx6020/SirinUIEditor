#pragma once
#include "FileOpenDialog.h"
#include <filesystem>

#include <thread>
#include <future>
#include <Graphics.h>

class Gui
{
public:
	Gui(Graphics& gfx)
		:
		gfx(gfx)
	{

	}
	Graphics& gfx;
	void RenderGui(void) noexcept;
private:
	void ShowCameraImporter();
	void SaveTexturesToBitmap();
	void SaveTexturesToDDS();
	int OutputButtons();
	bool pDemoWindowOpen = false;

	bool outputButtonsDisabled = false;
	std::future<int> future;

	std::filesystem::path fileInput {};
	std::filesystem::path folderOutput {};


	FileOpenDialog fileDialog;
};