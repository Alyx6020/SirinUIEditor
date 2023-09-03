#include "FileOpenDialog.h"
#include <cassert>
#include <string>
#include "Managers/FileOpenManager.h"


std::filesystem::path FileOpenDialog::Save(const std::string& filename, unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, int size, const FileType& type, const std::filesystem::path& folder)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	std::filesystem::path path{};

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, &pSaveDlg);

		if (rgSpec)
		{
			std::wstring stemp(filename.begin(), filename.end());

			pSaveDlg.Get()->SetOptions(dialogOptions);
			pSaveDlg.Get()->SetFileTypes(size, rgSpec);
			pSaveDlg.Get()->SetFileName(stemp.c_str());

			if (!folder.empty())
			{

				Microsoft::WRL::ComPtr<IShellItem> psiFolder;

				hr = SHCreateItemFromParsingName(folder.wstring().c_str(), nullptr, IID_PPV_ARGS(&psiFolder));

				if (SUCCEEDED(hr))
				{
					pSaveDlg->SetFolder(psiFolder.Get());
				}

			}

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pSaveDlg.Get()->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pSaveDlg.Get()->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Process file
						if (SUCCEEDED(hr))
						{
							path.assign(pszFilePath);

							FileOpenManager::Get().Save(filename, path, type);

							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pSaveDlg.Reset();
			}
			CoUninitialize();
		}
	}
	return path;
}

bool FileOpenDialog::SaveFolder(const std::string& filename, const FileType& type, unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, int size, std::filesystem::path* pathOut, const std::filesystem::path& folder)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	bool rtn = false;
	if (SUCCEEDED(hr))
	{
		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, &pOpenDlg);

		if (rgSpec)
		{
			pOpenDlg.Get()->SetOptions(dialogOptions);
			pOpenDlg.Get()->SetFileTypes(size, rgSpec);

			if (!folder.empty())
			{

				Microsoft::WRL::ComPtr<IShellItem> psiFolder;

				hr = SHCreateItemFromParsingName(folder.wstring().c_str(), nullptr, IID_PPV_ARGS(&psiFolder));

				if (SUCCEEDED(hr))
				{
					pOpenDlg->SetFolder(psiFolder.Get());
				}

			}
			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pOpenDlg.Get()->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pOpenDlg.Get()->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Process file
						if (SUCCEEDED(hr))
						{
							std::filesystem::path path(pszFilePath);

							//return 
							if (pathOut != nullptr)
							{
								*pathOut = path;
							}

							rtn = SUCCEEDED(hr);

							FileOpenManager::Get().Save(filename, path, type);

							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pOpenDlg.Reset();
			}
			CoUninitialize();

		}
	}
	return rtn;
}

bool FileOpenDialog::Open(unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, int size, std::filesystem::path* pathOut, ProcessFileFormat format)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	bool rtn = false;
	if (SUCCEEDED(hr))
	{
		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, &pOpenDlg);

		if (rgSpec)
		{
			pOpenDlg.Get()->SetOptions(dialogOptions);
			pOpenDlg.Get()->SetFileTypes(size, rgSpec);
			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pOpenDlg.Get()->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pOpenDlg.Get()->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Process file
						if (SUCCEEDED(hr))
						{
							std::filesystem::path path(pszFilePath);

							//return 
							if (pathOut != nullptr)
							{
								*pathOut = path;
							}

							rtn = SUCCEEDED(hr);

							FileOpenManager::Get().Open(path, format);

							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pOpenDlg.Reset();
			}
			CoUninitialize();

		}
	}
	return rtn;
}

