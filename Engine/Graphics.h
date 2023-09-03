#pragma once
#include "Define.h"
#include "Exception.h"
#include "DXError/DxgiInfo.h"
#include "GraphicsExceptionMacros.h"
#include <vector>
#include <d3d11_3.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>

class Graphics;

namespace graphics
{
	extern Graphics* gfx;
}

class Graphics
{
	friend class IBindable;
	friend class TextureAsset;
	friend class Gui;
public:
#pragma region EXCEPTIONS
	class GfxException : public Exception
	{
		using Exception::Exception;
	};

	// HRESULT EXCEPTION (FUNC RETURNS HRESULT)
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> dxInfo = {}) noexcept;
		const char* what() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	// DEVICE REMOVED EXCEPTION
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

	// DX INFO EXCEPTION (FUNC RETURNS VOID)
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
#pragma endregion Exception Classes

public:
	Graphics( HWND hWnd);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void Swap();
	void ClearBuffer(float r, float g, float b) noexcept;
	void DrawIndexed(UINT count) noexcept(DEBUG);

	HCURSOR cursor = LoadCursor(NULL, IDC_WAIT);
	void SetCursor()
	{
		::SetCursor(cursor);
	}


	//Projection
	void SetProjection(DirectX::FXMMATRIX projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	//Camera
	void SetCamera(DirectX::FXMMATRIX camera) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;


private:
	DirectX::XMMATRIX pProjection = {};
	DirectX::XMMATRIX pCamera;

#ifndef NDEBUG
	DxgiInfo pDxInfo;
#endif

public:

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice0;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap0;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext0;

	Microsoft::WRL::ComPtr<ID3D11Device3> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain2> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext3> pContext;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};

