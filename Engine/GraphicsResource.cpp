#include "Graphics.h"
#include "GraphicsResource.h"
#include <stdexcept>


ID3D11DeviceContext1* GraphicsResource::GetContext(Graphics& gfx) noexcept
{
	return gfx.pContext.Get();
}

ID3D11Device* GraphicsResource::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.Get();
}
