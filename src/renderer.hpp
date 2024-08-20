#pragma once
#include "common.hpp"
#include <imgui.h>

namespace big
{
	struct _FrameContext 
	{
		ID3D12CommandAllocator* m_command_allocator;
		ID3D12Resource* m_resource;
		D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor_handle;
	};
	class renderer
	{
	public:
		explicit renderer();
		~renderer();

		bool m_init = false;
		bool init(IDXGISwapChain3* swapchain);
		void imgui_init();

		void on_present(IDXGISwapChain3* swapchain);

		void pre_reset();
		void post_reset(IDXGISwapChain3* this_);
		void merge_icon_with_latest_font(float font_size, bool FontDataOwnedByAtlas = false);

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		ImFont* m_font = nullptr;
		ImFont* m_monospace_font = nullptr;

		HWND m_window = NULL;
		ID3D12CommandQueue* m_command_queue = nullptr;
	private:
		DXGI_SWAP_CHAIN_DESC m_swapchain_desc;
		ID3D12Device* m_d3d_device = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_rvt_handle;
		uint64_t m_buffer_count = -1;
		_FrameContext* m_frame_context = nullptr;
		ID3D12DescriptorHeap* m_descriptor_heap_backbuffer = nullptr;
		ID3D12DescriptorHeap* m_descriptor_heap_render = nullptr;
		ID3D12GraphicsCommandList* m_command_list = nullptr;
	};

	inline renderer *g_renderer{};
}
