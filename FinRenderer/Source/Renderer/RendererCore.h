#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>

#ifdef _DEBUG
#ifndef DXCALL
#define DXCall(x) x									 // \
//OutputDebugStringA("AHAHAHA");						 \
//if (FAILED(x))										 \
//{													 \
//	char line_number[32];							 \
//	sprintf_s(line_number, "%u", __LINE__);			 \
//	OutputDebugStringA("Error in:");  				 \
//	OutputDebugStringA(__FILE__);					 \
//	OutputDebugStringA("\nLine: ");					 \
//	OutputDebugStringA(line_number); 				 \
//	OutputDebugStringA("\n");						 \
//	OutputDebugStringA(#x);							 \
//	OutputDebugStringA("\n");						 \
//	__debugbreak();									 \
//}	
#endif								
#else
#ifndef DXCall
#define DXCall(x) x
#endif
#endif
