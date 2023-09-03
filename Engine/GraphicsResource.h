#pragma once
#include "Graphics.h"
#include <d3d11_1.h>

class Graphics;

class GraphicsResource
{
public:
	static ID3D11DeviceContext1* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
};