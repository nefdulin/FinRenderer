#pragma once
#include "Renderer/Renderer.h"

namespace FinRenderer
{
//	class Renderer;

	class Window
	{	
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		//Window() = default;

	public:
		Window(HINSTANCE hInstance, LPCWSTR className, LPCWSTR titleName, int nShowCmd);

		const HWND& GetWindowHandle() const { return m_hWindow; }

		void Run();
	private:
		bool InitializeWindow();

	private:
		Renderer* renderer;
		HWND m_hWindow;
		WNDCLASS m_WndClass;
		
		HINSTANCE m_hInstance;
		LPCWSTR m_ClassName;
		LPCUWSTR m_TitleName;
		int m_nShowCmd;
	};
}

