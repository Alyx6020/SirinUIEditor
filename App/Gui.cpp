#include "Gui.h"
#include "imgui.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "Managers\FileOpenManager.h"
#include "../Includes.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_stdlib.h"
#include <imgui_internal.h>
#include <fstream>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTex.h>
#include <wincodec.h>

#include <ImGui/imgui_canvas.h>
#include "AppSettings.h"
#include "Settings.h"
#include <Spr.h>
#include "Workspace.h"

// Text Editor
#include "ImGui/TextEditor.h"


static ImGuiEx::Canvas m_Canvas;
static TextEditor m_initEditor;
static TextEditor m_setspriteEditor;

static Frame* m_selected = nullptr;

bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Sptr");
	ImRect bb;
	bb.Min = 
	{
		window->DC.CursorPos.x + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1)).x,
		window->DC.CursorPos.y + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1)).y,
	};
		
	bb.Max =
	{ 
		bb.Min.x + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f).x,
		bb.Min.y + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f).y
	};
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis::ImGuiAxis_X : ImGuiAxis::ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

void Gui::RenderGui(void) noexcept
{
    

    ImVec2 offset = { 0,0 };
    ImU32 GRID_COLOR = IM_COL32_BLACK;
    float GRID_SX = 32.0f;
    float GRID_SY = 32.0f;
    ImVec2 VIEW_POS = { 0,0 };
    ImVec2 VIEW_SIZE = { WIDTH, HEIGHT };

    // Draw Grid
    auto m_DrawList = ImGui::GetBackgroundDrawList();

    m_DrawList->AddRectFilled(VIEW_POS, VIEW_POS + VIEW_SIZE, IM_COL32_G_SHIFT);

    for (float x = fmodf(offset.x, GRID_SX); x < VIEW_SIZE.x; x += GRID_SX)
        m_DrawList->AddLine(ImVec2(x, 0.0f) + VIEW_POS, ImVec2(x, VIEW_SIZE.y) + VIEW_POS, GRID_COLOR);
    for (float y = fmodf(offset.y, GRID_SY); y < VIEW_SIZE.y; y += GRID_SY)
        m_DrawList->AddLine(ImVec2(0.0f, y) + VIEW_POS, ImVec2(VIEW_SIZE.x, y) + VIEW_POS, GRID_COLOR);

    ImGui::ShowDemoWindow();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(VIEW_SIZE.x, 25));
    if (ImGui::Begin("transparent", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav))
    {


        if (ImGui::BeginMenuBar())
        {
            if (m_selected)
            {
                ImGui::SetNextItemWidth(100);
                ImGui::InputFloat("x", &m_selected->x);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::InputFloat("y", &m_selected->y);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::InputText("CGUIObject Name###test", &m_selected->varName);
                ImGui::SameLine();
                ImGui::Dummy(ImVec2(10, 10));

                static int item_current_2 = 0;
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("###combo 2 (one-liner)", &item_current_2, "Workspace\0Selection\0");

                if (ImGui::BeginMenu("Align"))
                {
                    if (ImGui::MenuItem("Horizontal Center", NULL))
                    {
                        m_selected->Center(m_workspaceTL, m_workspaceTL + m_workspaceSize);
                    }
                    if (ImGui::MenuItem("Vertical Center", NULL))
                    {

                    }
                    ImGui::EndMenu();
                }
            }

            
            
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

    if (ImGui::Begin("Code"))
    {
        m_initEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        m_setspriteEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());

        // Construct code

        std::string code = std::format("CGUIWindow::SetSize({},{});\n", m_workspaceSize.x, m_workspaceSize.y);

        if (!workspace::m_assets.empty())
        {
            // Set Sizes
            code += "\n//Set Size;\n";
            for (auto& iter : workspace::m_assets)
            {
                code += std::format("{}.SetSize({},{});\n", iter->varName, iter->width, iter->height);
            }

            // Set Pos
            code += "\n//Set Pos;\n";
            for (auto& iter : workspace::m_assets)
            {
                code += std::format("{}.SetPos({},{});\n", iter->varName, iter->x, iter->y);
            }

        }
        
        

        m_initEditor.SetText(code);
        static float size1 = 150;
        static float size2 = 300;

        ImGui::SeparatorText("Init");
        ImGui::BeginGroup();
        m_initEditor.Render("test", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y/2));
        ImGui::EndGroup();
        
        ImGui::SeparatorText("SetSprite");
        ImGui::BeginGroup();
        m_setspriteEditor.Render("test2", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));
        ImGui::EndGroup();
        

        
    }
    ImGui::End();

    if (ImGui::Begin("Menu"))
    {
        

        // --- Workspace size ----------------------------------------------------------
        ImGui::InputFloat("Width", &m_workspaceSize.x, 1, 10);
        ImGui::InputFloat("Height", &m_workspaceSize.y, 1, 10);
        ImGui::Separator();

        // --- File Paths ----------------------------------------------------------
        {
            COMDLG_FILTERSPEC rgSpec[] =
            {
                {L"RF SPT (*.spt)",       L"*.dat"}
            };

            ImGui::SeparatorText("Client SpriteImage Path");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (60));

            static std::string datatablePath = settings::spriteImagePath.string();

            if (ImGui::InputText("###settings-datatable-path-input", &datatablePath))
            {
                settings::spriteImagePath = datatablePath;
            }

            ImGui::PopItemWidth();
            ImGui::SameLine();

            if (ImGui::Button("Open###settings-sprite-path-button"))
            {
                if (FileOpenDialog::Get().Open(FOS_PICKFOLDERS, rgSpec, 1, &settings::spriteImagePath))
                {
                    datatablePath = settings::spriteImagePath.string();
                }
            }

            if (ImGui::Button("Save###settings-sprite-path-save", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                settings::Update();
            }
        }

        {
            ImGui::SeparatorText("SPR List");
            if (ImGui::Button("Load SPR's", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                spr::Load();
            }
            spr::Render();
        }

        // --- Test Open Spr ----------------------------------------------------------

        {
            COMDLG_FILTERSPEC rgSpec[] =
            {
                {L"RF SPR (*.spr)",       L"*.spr"}
            };

            if (ImGui::Button("Open###spr_open"))
            {
                if (FileOpenDialog::Get().Open(FOS_STRICTFILETYPES, rgSpec, 1, nullptr))
                {
                }
            }
        }
    }
    ImGui::End();

    auto style = ImGui::GetStyle();

    ImGui::SetNextWindowSize(m_workspaceSize + style.WindowPadding * 2);
    ImGui::SetNextWindowPos(
        ImVec2(
            WIDTH * 0.5 - (m_workspaceSize.x * 0.5),
            HEIGHT * 0.5 - (m_workspaceSize.y * 0.5)
        ));

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Workspace", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav))
    {
        auto areaTL = m_workspaceTL = ImGui::GetCursorScreenPos();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        
        ImGui::BeginChild("area", m_workspaceSize, true, ImGuiWindowFlags_NoScrollbar);

        auto list = ImGui::GetForegroundDrawList();

        for (auto& it : workspace::m_assets)
        {
            ImGui::SetCursorPos({ it->x, it->y });

            auto cursor = ImGui::GetCursorScreenPos();

            ImGui::Image((void*)it->srv.Get(), ImVec2(it->xwidth, it->xheight));
            ImGui::SetItemAllowOverlap();

            if (m_selected == it.get())
            {
                list->AddRect(
                    ImVec2(
                        it->x + areaTL.x,
                        it->y + areaTL.y
                    ),
                    ImVec2(
                        it->x + areaTL.x + it->width,
                        it->y + areaTL.y + it->height
                    ), ImColor(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f));
            }
            
            
            if ((ImGui::IsMouseDown(ImGuiMouseButton_Left)) && m_selected == it.get() && ImGui::IsWindowHovered())
            {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    auto x = ImGui::GetMousePos().x - areaTL.x - it->tempx;
                    auto y = ImGui::GetMousePos().y - areaTL.y - it->tempy;

                    x = std::clamp(x, 0.0f, std::max(0.0f, m_workspaceSize.x - it->width));
                    y = std::clamp(y, 0.0f, std::max(0.0f, m_workspaceSize.y - it->height));

                    it->x = x;
                    it->y = y;
                }

            }

            // Update Pos
            it->rect.x = it->x;
            it->rect.y = it->y;
            it->cursorx = cursor.x;
            it->cursory = cursor.y;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
        {
            bool found = false;
            for (auto it = workspace::m_assets.rbegin(); it != workspace::m_assets.rend(); ++it)
            {
                auto x = ImGui::GetMousePos().x - areaTL.x;
                auto y = ImGui::GetMousePos().y - areaTL.y;
                
                if (it->get()->rect.contains({ x, y }))
                {
                    m_selected = it->get();

                    it->get()->tempx = ImGui::GetMousePos().x - it->get()->cursorx;
                    it->get()->tempy = ImGui::GetMousePos().y - it->get()->cursory;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                m_selected = nullptr;
            }
        }

        ImGui::EndChild();

        ImGui::PopStyleVar();

        if (ImGui::BeginDragDropTarget())
        {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {

            }

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_RFS"))
            {
                spr::SprPayload header = *static_cast<const spr::SprPayload*>(payload->Data);


                auto x = ImGui::GetMousePos().x - areaTL.x;
                auto y = ImGui::GetMousePos().y - areaTL.y;
                printf("%f %f\n", x, y);

                workspace::Add(header.path, header.group, header.frame, x, y);

            }
            ImGui::EndDragDropTarget();
        }
    }
    ImGui::End();
    //ImGui::PopStyleVar();

    // --- Workspace DDS List ----------------------------------------------------------
    if (ImGui::Begin("Layers"))
    {
        
        static std::vector<std::string> item_names = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
        
        
        for (int n = workspace::m_assets.size() - 1; n >= 0; n--)
        {
            auto item = item_names[n];

            auto &iter = workspace::m_assets.at(n);

            ImGui::Selectable(std::string(iter->path.stem().string() + " " + std::to_string(iter->group) + " - " + std::to_string(iter->frame)).c_str());


            if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
            {
                int n_next = n + (ImGui::GetMouseDragDelta(0).y > 0.f ? -1 : 1);
                if (n_next >= 0 && n_next < workspace::m_assets.size())
                {
                    std::swap(workspace::m_assets[n], workspace::m_assets[n_next]);
                    ImGui::ResetMouseDragDelta();
                }
            }
        }

        /*int i = workspace::m_assets.size();
        for (auto iter = workspace::m_assets.rbegin(); iter != workspace::m_assets.rend(); ++iter)
        {
            ImGui::PushID(i);


            ImGui::Selectable(std::string(iter->get()->path.stem().string() + " " + std::to_string(iter->get()->group) + " - " + std::to_string(iter->get()->frame)).c_str());

            if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
            {
                int n_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                if (n_next >= 0 && n_next < workspace::m_assets.size())
                {
                    std::swap(workspace::m_assets[i], workspace::m_assets[n_next]);
                    ImGui::ResetMouseDragDelta();
                }
            }

            ImGui::PopID();
            i--;
        }*/

        
    }
    ImGui::End();


}

void Gui::SaveTexturesToBitmap()
{
    auto ptr = FileOpenManager::Get().Get(FileType::R3T, fileInput.stem().string());

    if (ptr)
    {
        auto it = reinterpret_cast<R3TFile*>(ptr);

        future = std::async(std::launch::async, [&, it]()
            {
                for (int i = 0; i < it->headers.size(); i++)
                {
                    size_t mipLevels = 0;
                    DirectX::TexMetadata info;
                    std::unique_ptr<DirectX::ScratchImage> image(new (std::nothrow) DirectX::ScratchImage);

                    HRESULT hr = DirectX::LoadFromDDSMemory(it->data[i].data(), it->data[i].size(), DirectX::DDS_FLAGS_NONE, &info, *image.get());

                    if (FAILED(hr))
                    {
                        continue;
                    }

                    size_t tMips = (!mipLevels && info.mipLevels > 1) ? info.mipLevels : mipLevels;

                    // --- Decompress --------------------------------------------------------------

                    if (i == 136)
                    {
                        auto a = it->headers[i].filename;
                    }
                    
                    if (DirectX::IsCompressed(info.format))
                    {
                        if ((info.width % 4) != 0 || (info.height % 4) != 0)
                        {
                            return 0;
                        }

                        const DirectX::Image* img = image->GetImage(0, 0, 0);
                        assert(img);
                        size_t nimg = image->GetImageCount();

                        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);

                        hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage);

                        if (FAILED(hr))
                        {
                            return 0;
                        }

                        auto& tinfo = timage->GetMetadata();
                        info.format = tinfo.format;

                        image.swap(timage);
                    }

                    {
                        std::unique_ptr<DirectX::ScratchImage> timage(new (std::nothrow) DirectX::ScratchImage);

                        DirectX::TexMetadata mdata = info;
                        mdata.mipLevels = 1;
                        hr = timage->Initialize(mdata);

                        for (size_t i = 0; i < info.arraySize; ++i)
                        {
                            hr = CopyRectangle(*image->GetImage(0, i, 0), DirectX::Rect(0, 0, info.width, info.height),
                                *timage->GetImage(0, i, 0), DirectX::TEX_FILTER_DEFAULT, 0, 0);
                            if (FAILED(hr))
                            {
                                return 0;
                            }
                        }

                        image.swap(timage);
                        info.mipLevels = 1;
                    }
                    


                    // --- Set alpha mode ----------------------------------------------------------
                    if (DirectX::HasAlpha(info.format)
                        && info.format != DXGI_FORMAT_A8_UNORM)
                    {
                        if (image->IsAlphaAllOpaque())
                        {
                            info.SetAlphaMode(DirectX::TEX_ALPHA_MODE_OPAQUE);
                        }
                        else if (info.IsPMAlpha())
                        {
                            // Aleady set TEX_ALPHA_MODE_PREMULTIPLIED
                        }
                    }
                    else
                    {
                        info.SetAlphaMode(DirectX::TEX_ALPHA_MODE_UNKNOWN);
                    }

                    // --- Save result -------------------------------------------------------------
                    std::filesystem::path path = folderOutput.empty() ? fileInput.parent_path() : folderOutput;
                    std::filesystem::path file(it->headers[i].filename);

                    path.append(it->name);
                    path.append("tex");
                    std::filesystem::create_directories(path);

                    path.append(file.filename().replace_extension(".bmp").string());
                    
                    {
                        auto img = image->GetImage(0, 0, 0);
                        assert(img);
                        size_t nimg = image->GetImageCount();

                        hr = DirectX::SaveToWICFile(img, 1, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_BMP), path.wstring().c_str(), nullptr);
                    }
                }
                return 1;
            });
    }
}

void Gui::SaveTexturesToDDS()
{
    auto ptr = FileOpenManager::Get().Get(FileType::R3T, fileInput.stem().string());

    if (ptr)
    {
        auto it = reinterpret_cast<R3TFile*>(ptr);

        future = std::async(std::launch::async, [&, it]()
            {
                for (int i = 0; i < it->headers.size(); i++)
                {
                    std::filesystem::path path = folderOutput.empty() ? fileInput.parent_path() : folderOutput;
                    std::filesystem::path file(it->headers[i].filename);

                    path.append(it->name);
                    path.append("tex");
                    std::filesystem::create_directories(path);

                    path.append(file.filename().string());
                    std::ofstream out(path.string().c_str(), std::ios_base::binary);


                    if (out.is_open())
                    {
                        out.write((char*)it->data[i].data(), it->data[i].size());
                        out.close();
                    }
                }

                return 1;
            });
    }
}

int Gui::OutputButtons()
{
    if (outputButtonsDisabled)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    if (ImGui::Button("To DDS", ImVec2(80, 19)))
    {
        FileOpenManager::Get().Open(fileInput);
        outputButtonsDisabled = true;
        return 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("To BMP", ImVec2(80, 19)))
    {
        FileOpenManager::Get().Open(fileInput);
        outputButtonsDisabled = true;
        return 2;
    }

    if (outputButtonsDisabled)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    return 0;
}