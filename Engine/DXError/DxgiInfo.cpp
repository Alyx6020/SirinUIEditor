#pragma comment(lib,"dxguid.lib")

#include "DxgiInfo.h"
#include "Window/Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>
#include "Window/WindowExceptionMacros.h"


DxgiInfo::DxgiInfo()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32 );
	if (hModDxgiDebug == nullptr)
	{
		throw ENGINE_LAST_EXCEPTION();
	}
	
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw ENGINE_LAST_EXCEPTION();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue),&pDxgiInfoQueue));
}

void DxgiInfo::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(pDebugType);
}

std::vector<std::string> DxgiInfo::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(pDebugType);
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;

		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(pDebugType, i, nullptr, &messageLength));
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(pDebugType, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}
