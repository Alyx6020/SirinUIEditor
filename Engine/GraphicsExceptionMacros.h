#pragma once

//Exceptions without DX_DEBUG
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

//Exceptions with DX_DEBUG
#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),pDxInfo.GetMessages() )
#define GFX_THROW_INFO(hrcall) pDxInfo.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),pDxInfo.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) pDxInfo.Set(); (call); {auto v = pDxInfo.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

//DXInfo
#ifdef NDEBUG
#define GFX_THROW_SETUP(gfx) HRESULT hr
#else
#define GFX_THROW_SETUP(gfx) HRESULT hr; DxgiInfo& pDxInfo = GetDxgiInfo(gfx)
#endif
