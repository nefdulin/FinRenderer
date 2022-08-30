#pragma once
#include "RendererCore.h"

namespace FinRenderer
{
	class Window;

	class Renderer
	{
	public:
		Renderer();

		Renderer(Window* window) :
			m_CurrentBackBufferIndex(0), m_CurrentFence(0)
		{
			m_Window = window;
		}

		void Initialize();
		void Render();

		ID3D12Resource* GetCurrentBackBuffer();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilBufferView() const;

		void FlushCommandQueue();
	private:
		IDXGIAdapter1* GetHardwareAdapter(D3D_FEATURE_LEVEL featureLevel);

	private:
		const static uint32_t m_SwapChainBufferCount = 2;

		Window* m_Window;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device8> m_Device;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffers[m_SwapChainBufferCount];

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		uint64_t m_CurrentFence;

		uint32_t m_DescriptorSizeRTV;
		uint32_t m_DescriptorSizeDSV;
		uint32_t m_DescriptorSizeCBVSRVUAV;


#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> m_DebugController;
#endif

		int m_CurrentBackBufferIndex;
	};
}

