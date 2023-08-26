#pragma once
#include "Define.h"
#include <ShlObj.h>
#include <wrl.h>
#include <string>
#include<functional> 

class FileOpenDialog
{
public:
    FileOpenDialog() = default;
    int Open(unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, size_t size, const std::function <void(std::string)>& f);

private:
    Microsoft::WRL::ComPtr<IFileOpenDialog> pOpenDlg;

};