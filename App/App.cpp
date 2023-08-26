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



App::App() : 
	wnd(WIDTH, HEIGHT,"BSP ExportTool"),
	gui(wnd.Gfx())
{
}

int App::Start()
{
	while (true)
	{
		if (const auto exitCode = Window::ProcessMessages())
		{
			return *exitCode;
		}
		AdvanceFrame();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void App::AdvanceFrame()
{
	wnd.Gfx().ClearBuffer(0.1, 0.1, 0.1);

	//ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	gui.RenderGui();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	wnd.Gfx().Swap();
}
