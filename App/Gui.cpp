#include "Gui.h"
#include "imgui.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "Managers\FileOpenManager.h"
#include "../Includes.h"
#include <imgui_internal.h>
#include <fstream>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTex.h>
#include <wincodec.h>


void Gui::RenderGui(void) noexcept
{
    ImGui::SetNextWindowSize(ImVec2(WIDTH,HEIGHT), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if (ImGui::Begin("Texture Converter",0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
    {
        //---------------------------------------------------------
        //  Select r3t file
        //---------------------------------------------------------
        
        ImGui::Text("Extract Textures: (.r3t)");

        ImGui::SeparatorText("test");

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 40 - ImGui::GetStyle().WindowPadding.x);
        ImGui::InputText("##texture-extract-str", (char*)fileInput.string().c_str(), fileInput.string().size() + 1);
        ImGui::SameLine();
        if (ImGui::Button("...##input", ImVec2(40, 19)))
        {
            COMDLG_FILTERSPEC rgSpec[] =
            {
                { L"RF Texture (*.r3t)", L"*.r3t" }
            };

            fileDialog.Open(FOS_STRICTFILETYPES, rgSpec, 1, [&](std::string str)
            {
                    fileInput = str;
            });
        }

        //---------------------------------------------------------
        //  Select output folder
        //---------------------------------------------------------

        ImGui::Text("Output folder: (optional)");

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 40 - ImGui::GetStyle().WindowPadding.x);
        ImGui::InputText("##texture-output-str", (char*)folderOutput.string().c_str(), folderOutput.string().size() + 1);
        ImGui::SameLine();
        if (ImGui::Button("...##output", ImVec2(40, 19)))
        {
            COMDLG_FILTERSPEC rgSpec[] =
            {
                { L"RF Texture (*.r3t)", L"*.r3t" }
            };

            fileDialog.Open(FOS_PICKFOLDERS, rgSpec, 1, [&](std::string str)
                {
                    folderOutput = str;
                });
        }

        //---------------------------------------------------------
        //  Buttons
        //---------------------------------------------------------

        ImGui::Dummy(ImVec2(0, 5));

        auto btn = OutputButtons();
        if (btn)
        {
            gfx.SetCursor();
            switch (btn)
            {
            case 1:
            {
                SaveTexturesToDDS();
                break;
            }
            case 2:
            {
                SaveTexturesToBitmap();
                break;
            }
            }
        }

        if (future._Is_ready())
        {
            FileOpenManager::Get().Erase(FileType::R3T, fileInput.stem().string());

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            future.get();
            outputButtonsDisabled = false;
        }
        else
        {
            if (outputButtonsDisabled)
            {
                SetCursor(LoadCursor(NULL, IDC_WAIT));
            }
        }
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