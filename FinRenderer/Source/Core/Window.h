#pragma once
#include <Windows.h>

namespace FinRenderer
{
	class Window
	{	
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		Window() = default;

	public:
		Window(HINSTANCE hInstance, LPCWSTR className, LPCWSTR titleName, int nShowCmd);

		void Run();
	private:
		bool InitializeWindow();

	private:
		HWND m_hWindow;
		WNDCLASS m_WndClass;
		
		HINSTANCE m_hInstance;
		LPCWSTR m_ClassName;
		LPCUWSTR m_TitleName;
		int m_nShowCmd;
	};
}

