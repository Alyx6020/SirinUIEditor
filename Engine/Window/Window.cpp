#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowExceptionMacros.h"
#include "ImGui\imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() :
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = CS_DBLCLKS;
	wc.hInstance = GetInstance();
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;
	wc.lpszClassName = GetName();
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = HandleMsgSetup;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE  Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

///

Window::Window(int width, int height, const char* name) : width(width), height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr)
	{
		throw ENGINE_LAST_EXCEPTION();
	}

	//Init imgui
	ImGui_ImplWin32_Init(hWnd);

	ShowWindow(hWnd, SW_SHOWDEFAULT);



	pGraphics = std::make_unique<Graphics>(hWnd);

	//Register Raw Input https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevice
	RAWINPUTDEVICE rid = {};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw ENGINE_LAST_EXCEPTION();
	}
}


Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return static_cast<int>(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

void Window::SetTitle(const std::string& title)
{
	SetWindowText(hWnd, title.c_str());
}

Graphics& Window::Gfx()
{
	return *pGraphics;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lparam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hwnd, msg, wparam, lparam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
		/// Exit
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_KILLFOCUS:
		{
			kbd.ClearState();
			break;
		}
		/// Keyboard
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (!(lParam & 40000000) || kbd.AutoRepeatIsEnabled())
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;

		///Raw Mouse Input
		case WM_INPUT:
		{
			uint32_t size = 0;
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}

			rawBuffer.resize(size);

			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				break;
			}

			auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (rawInput.header.dwType == RIM_TYPEMOUSE)
			{
				if (rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0)
				{
					mouse.OnRawDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
				}

				const float delta = (float)WHEEL_DELTA * 5;
				const float mouseWheelData = (float)((short)LOWORD(rawInput.data.mouse.usButtonData));

				mouse.mouseWheelDelta = std::max(0.0f, mouse.mouseWheelDelta + mouseWheelData / delta);
				
			}
			break;
		}

		///Mouse
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// in client region -> log move, and log enter + capture mouse
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{
				mouse.OnMouseMove(pt.x, pt.y);
				if (!mouse.IsInWindow())
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
				break;
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}
			break;
		}
		case WM_SETCURSOR:
		{
			if ((HWND)wParam == GetDlgItem(hWnd, 4000))
			{
				SetCursor(LoadCursor(NULL, IDC_WAIT));
				return TRUE;
			}
		}
	}
	

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

///

Window::WindowException::WindowException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{
}

const char* Window::WindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code: " << GetErrorCode() << std::endl
		<< "Description: " << GetErrorString() << std::endl
		<< GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Window::WindowException::GetType() const noexcept
{
	return Exception::GetType();
}

std::string Window::WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	unsigned long nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified Error";
	}

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::WindowException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::WindowException::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
