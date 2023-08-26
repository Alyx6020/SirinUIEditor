#pragma once

#define ENGINE_EXCEPTION(hr) Window::WindowException(__LINE__,__FILE__,hr);
#define ENGINE_LAST_EXCEPTION() Window::WindowException(__LINE__,__FILE__,GetLastError());
