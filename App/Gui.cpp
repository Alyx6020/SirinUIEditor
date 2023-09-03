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


static ImGuiEx::Canvas m_Canvas;


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
    

    if (ImGui::Begin("Workspace", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::BeginChild("area", m_workspaceSize, true);

        

        ImGui::EndChild();
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