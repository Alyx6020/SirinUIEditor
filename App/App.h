#pragma once
#include "Window/Window.h"
#include "ImGui/ImGuiManager.h"
#include "../Includes.h"
#include "Gui.h"


class App
{
public:
	App();
	int Start();
private:
	void AdvanceFrame();
private:
	ImGuiManager imgui;
	Window wnd;
	Gui	  gui;
};

