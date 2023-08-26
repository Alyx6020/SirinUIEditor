#pragma once

#include "Define.h"
#include <vector>
#include <dxgidebug.h>
#include <string>
#include <wrl.h>

class DxgiInfo
{
public:
	DxgiInfo();
	~DxgiInfo() = default;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	const GUID pDebugType = DXGI_DEBUG_DX;
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};