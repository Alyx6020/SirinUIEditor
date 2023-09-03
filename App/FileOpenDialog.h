#pragma once
#include "Define.h"
#include <ShlObj.h>
#include <wrl.h>
#include <string>
#include "Managers/FileOpenManager.h"
#include <filesystem>


class FileOpenDialog
{
public:
    static FileOpenDialog& Get()
    {
        static FileOpenDialog instance;
        return instance;
    }

    bool Open(unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, int size, std::filesystem::path* pathOut = nullptr, ProcessFileFormat format = ProcessFileFormat::PATH);
    std::filesystem::path Save(const std::string& filename, unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, int size, const FileType& type = FileType::NUL, const std::filesystem::path& folder = {});
    bool SaveFolder(const std::string& filename, const FileType& type, unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec = NULL, int size = 0, std::filesystem::path* pathOut = {}, const std::filesystem::path& folder = {});

    FileOpenDialog() = default;
    FileOpenDialog(FileOpenDialog const&) = delete;
    void operator=(FileOpenDialog const&) = delete;

private:
    Microsoft::WRL::ComPtr<IFileOpenDialog> pOpenDlg;
    Microsoft::WRL::ComPtr<IFileSaveDialog> pSaveDlg;
};
