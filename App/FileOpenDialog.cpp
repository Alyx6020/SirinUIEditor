#include "FileOpenDialog.h"
#include <cassert>
#include <string>
#include "Managers/FileOpenManager.h"


int FileOpenDialog::Open(unsigned long dialogOptions, COMDLG_FILTERSPEC* rgSpec, size_t size, const std::function <void(std::string)>& f)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

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

							f(path.string());


							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pOpenDlg.ReleaseAndGetAddressOf();
			}
			CoUninitialize();

			return 0;
		}

	}
}