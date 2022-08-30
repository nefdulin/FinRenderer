#include "stdafx.h"
#include "Window.h"
#include "Renderer/Renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	std::shared_ptr<FinRenderer::Window> window = std::make_shared<FinRenderer::Window>(hInstance, L"BasicWindowClass", L"Fin Renderer", nShowCmd);
	
	// 1.) Create a DXGI factor
	// 2.) Use the factory to find a suitable adapter that supports d3d12
	// 3.) Create a device using the adapter
	// 4.) Create the swap chain descriptor, descriptor heap and the swap chain itself
	// 5.) Create command queue, command list and allocator
	// 6.) Create viewport and scissor

	window->Run();

	return 0;

}

