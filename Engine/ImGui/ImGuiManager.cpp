#include "ImGuiManager.h"
#include "imgui.h"

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    style->WindowRounding = 2.0f;
    style->ScrollbarRounding = 3.0f;
    style->GrabRounding = 2.0f;
    style->AntiAliasedLines = true;
    style->AntiAliasedFill = true;
    style->WindowRounding = 2;
    style->WindowPadding = ImVec2(10, 10);
    style->ChildRounding = 2;
    style->ScrollbarSize = 10;
    style->ScrollbarRounding = 3;
    style->GrabRounding = 2;
    style->ItemSpacing.x = 10;
    style->ItemSpacing.y = 5;
    style->IndentSpacing = 22;
    style->FramePadding.x = 6;
    style->FramePadding.y = 4;
    style->Alpha = 1.0f;
    style->FrameRounding = 3.0f;

    style->WindowBorderSize = 1.0f;
    style->ChildBorderSize = 1.0f;
    style->FrameBorderSize = 0.0f;
    style->PopupBorderSize = 1.0f;

    colors[ImGuiCol_Text] = ImVec4(1.0, 1.0, 1.0, 1.0);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434, 0.5921568870544434, 0.5921568870544434, 1.0);
    colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_Border] = ImVec4(0.1058823645114899, 0.1058823645114899, 0.1058823645114899, 1.0);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899, 0.3058823645114899, 0.3058823645114899, 1.0);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2156862765550613, 1.0);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.1000000029802322, 0.1000000029802322, 0.1000000029802322, 1.0);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2156862765550613, 1.0);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319, 0.321568638086319, 0.3333333432674408, 1.0);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_CheckMark] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_Button] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2156862765550613, 1.0);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_Header] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2156862765550613, 1.0);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899, 0.3058823645114899, 0.3058823645114899, 1.0);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899, 0.3058823645114899, 0.3058823645114899, 1.0);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2156862765550613, 1.0);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319, 0.321568638086319, 0.3333333432674408, 1.0);
    colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_TabActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_PlotLines] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.2000000029802322, 1.0);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971, 0.3098039329051971, 0.3490196168422699, 1.0);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832, 0.2274509817361832, 0.2470588237047195, 1.0);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0, 0.0, 0.0, 0.0);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0, 1.0, 1.0, 0.05999999865889549);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0, 0.4666666686534882, 0.7843137383460999, 1.0);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.1137254908680916, 0.5921568870544434, 0.9254902005195618, 1.0);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 1.0);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0, 1.0, 1.0, 0.699999988079071);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929, 0.800000011920929, 0.800000011920929, 0.2000000029802322);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304, 0.1450980454683304, 0.1490196138620377, 0.8);
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}
