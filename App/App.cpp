#include "App.h"
#include <sstream>
#include <iomanip>
#include <memory>
#include "imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"
#include <chrono>
#include <thread>
#include <Managers/FileOpenManager.h>
#include <AppSettings.h>



App::App() : 
	wnd(WIDTH, HEIGHT,"Sirin UI Editor"),
	gui(wnd.Gfx())
{
}

int App::Start()
{
	settings::Load();
	while (true)
	{
		if (const auto exitCode = Window::ProcessMessages())
		{
			return *exitCode;
		}
		AdvanceFrame();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void App::AdvanceFrame()
{
	wnd.Gfx().ClearBuffer(0.1, 0.1, 0.1);
	
	if (wnd.mouse.IsInWindow())
	{
		gui.m_zoom = wnd.mouse.mouseWheelDelta;
	}
	

	//ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	gui.RenderGui();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	wnd.Gfx().Swap();
}
