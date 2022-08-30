#include "stdafx.h"
#include "Window.h"
#include "Renderer/Renderer.h"

namespace FinRenderer
{
	Window::Window(HINSTANCE hInstance, LPCWSTR className, LPCWSTR titleName, int nShowCmd) : 
		m_hInstance(hInstance), m_ClassName(className), m_TitleName(titleName), m_nShowCmd(nShowCmd)
	{
		m_WndClass.style = CS_HREDRAW | CS_VREDRAW;
		m_WndClass.lpfnWndProc = Window::WndProc;
		m_WndClass.cbClsExtra = 0;
		m_WndClass.cbWndExtra = 0;
		m_WndClass.hInstance = m_hInstance;
		m_WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		m_WndClass.hCursor = LoadCursor(0, IDC_ARROW);
		m_WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		m_WndClass.lpszMenuName = 0;
		m_WndClass.lpszClassName = m_ClassName;

		if (!InitializeWindow())
		{
			MessageBox(0, L"Window Initialization FAILED", 0, 0);
			return;
		}
	}

	bool Window::InitializeWindow()
	{
		if (!RegisterClass(&m_WndClass))
		{
			MessageBox(0, L"RegisterClass FAILED", 0, 0);
			return false;
		}

		m_hWindow = CreateWindow(
			m_ClassName,
			m_TitleName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			m_hInstance,
			0
		);

		if (m_hWindow == 0)
		{
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		ShowWindow(m_hWindow, m_nShowCmd);
		UpdateWindow(m_hWindow);

		renderer = new Renderer(this);
		renderer->Initialize();
	}

	void Window::Run()
	{
		MSG msg = { 0 };

		renderer->Render();

		BOOL bRet = 1;
		while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
		{
			if (bRet == -1)
			{
				MessageBox(0, L"GetMessage FAILED", L"Error", MB_OK);
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	
	LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN:
		{
			MessageBox(0, L"Hello WORLD", L"Hello", MB_OK);
			return 0;
		}
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
				DestroyWindow(hWnd);

			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}