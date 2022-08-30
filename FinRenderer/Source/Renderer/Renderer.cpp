#include "stdafx.h"
#include "Renderer.h"
#include "Core/Window.h"

namespace FinRenderer
{
	using namespace Microsoft::WRL;
	
	void Renderer::Initialize()
	{
#ifdef _DEBUG
		DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController)));

		m_DebugController->EnableDebugLayer();
#endif

		uint32_t dxgiFactoryFlags { 0 };

#ifdef _DEBUG
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		DXCall(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_DXGIFactory)));

		m_Adapter = GetHardwareAdapter(D3D_FEATURE_LEVEL_11_0);

		DXCall(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));

		m_DescriptorSizeRTV = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DescriptorSizeDSV = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_DescriptorSizeCBVSRVUAV = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		DXCall(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		DXCall(m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));
		DXCall(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
		DXCall(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));

		m_SwapChain.Reset();
		
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 1280;
		swapChainDesc.BufferDesc.Height = 960;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_SwapChainBufferCount;
		swapChainDesc.OutputWindow = m_Window->GetWindowHandle();
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DXCall(m_DXGIFactory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, m_SwapChain.GetAddressOf()));

		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.NumDescriptors = m_SwapChainBufferCount;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;

		DXCall(m_Device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_RTVHeap.GetAddressOf())));

		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.NumDescriptors = 1;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;

		DXCall(m_Device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(m_DSVHeap.GetAddressOf())));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart());

		for (uint32_t i = 0; i < m_SwapChainBufferCount; i++)
		{
			DXCall(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffers[i])));

			m_Device->CreateRenderTargetView(m_SwapChainBuffers[i].Get(), nullptr, rtvHeapHandle);

			rtvHeapHandle.Offset(1, m_DescriptorSizeRTV);
		}

		D3D12_RESOURCE_DESC depthStencilDesc;

		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = 1280;
		depthStencilDesc.Height = 960;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0.0f;

		auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		DXCall(m_Device->CreateCommittedResource(
			&heapType,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())
		));

		m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

		auto depthStencilBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_CommandList->ResourceBarrier(1, &depthStencilBufferTransition);

		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = static_cast<float>(1280);
		m_Viewport.Height = static_cast<float>(960);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_CommandList->RSSetViewports(1, &m_Viewport);

		m_ScissorRect = { 0, 0, 1280 / 2, 960 / 2 };
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		DXCall(m_CommandList->Close());

		ID3D12CommandList* commandLists[] = {m_CommandList.Get()};
		m_CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	void Renderer::Render()
	{
		DXCall(m_CommandAllocator->Reset());

		DXCall(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

		auto backBufferBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_CommandList->ResourceBarrier(1, &backBufferBarrier);

		m_CommandList->RSSetViewports(1, &m_Viewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		FLOAT colors[] = { 1.0f, 0.1f, 0.5f, 1.0f };
		m_CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), colors, 0, nullptr);
		m_CommandList->ClearDepthStencilView(GetDepthStencilBufferView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0.0f, 0, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE x, y;
		m_CommandList->OMSetRenderTargets(1, &(x = GetCurrentBackBufferView()), true, &(y = GetDepthStencilBufferView()));

		auto backBufferBarrier2 = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandList->ResourceBarrier(1, &backBufferBarrier2);
		
		DXCall(m_CommandList->Close());

		ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		DXCall(m_SwapChain->Present(0, 0));
		m_CurrentBackBufferIndex = (m_CurrentBackBufferIndex + 1) % m_SwapChainBufferCount;

		FlushCommandQueue();
	}

	void Renderer::FlushCommandQueue()
	{
		m_CurrentFence++;
		DXCall(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

		if (m_Fence->GetCompletedValue() < m_CurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, 0, false, EVENT_ALL_ACCESS);

			DXCall(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	ID3D12Resource* Renderer::GetCurrentBackBuffer()
	{
		return m_SwapChainBuffers[m_CurrentBackBufferIndex].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetCurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_DescriptorSizeRTV);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilBufferView() const
	{
		return m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
	}

	IDXGIAdapter1* Renderer::GetHardwareAdapter(D3D_FEATURE_LEVEL featureLevel)
	{
		if (!m_DXGIFactory)
		{
			OutputDebugStringA("Need to initialize renderer before calling GetHardwareAdapter");
			return nullptr;
		}
		
		for (uint32_t adapterIndex = 0; ; adapterIndex++)
		{
			IDXGIAdapter1* currentAdapter = nullptr;
			if ((m_DXGIFactory->EnumAdapters1(adapterIndex, &currentAdapter)) == DXGI_ERROR_NOT_FOUND)
			{
				OutputDebugStringA("Couldn't find suitable hardware adapter!");
				break;
			}

			HRESULT hr{ S_OK };
			DXCall(hr = D3D12CreateDevice(currentAdapter, featureLevel, __uuidof(ID3D12Device), nullptr));
			if (hr)
			{
				return currentAdapter;
			}

			currentAdapter->Release();
		}

		return nullptr;
	}
}