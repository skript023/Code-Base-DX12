#pragma once
#include "common.hpp"
#include <imgui.h>

namespace big
{
	using dx_callback      = std::function<void()>;
	using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

	class renderer
	{
		struct _FrameContext 
		{
			ID3D12CommandAllocator* m_command_allocator;
			ID3D12Resource* m_resource;
			D3D12_CPU_DESCRIPTOR_HANDLE m_descriptor_handle;
		};
	public:
		explicit renderer();
		~renderer();

		/**
		 * @brief Add a callback function to draw your ImGui content in
		 * 
		 * @param callback Function
		 * @param priority The higher the priority the value the later it gets drawn on top
		 * @return true 
		 * @return false 
		 */
		bool add_dx_callback(dx_callback callback, std::uint32_t priority);
		/**
		 * @brief Add a callback function on wndproc
		 * 
		 * @param callback Function
		 */
		void add_wndproc_callback(wndproc_callback callback);
		bool init(IDXGISwapChain3* swapchain);
		void imgui_init();

		void on_present(IDXGISwapChain3* swapchain);
		void rescale(float rel_size);
		void pre_reset();
		void post_reset();
		void merge_icon_with_latest_font(float font_size, bool FontDataOwnedByAtlas = false);

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		void new_frame();
		void end_frame(IDXGISwapChain3* swapchain);
	public:
		bool m_init = false;
		ImFont* m_font = nullptr;
		ImFont* m_monospace_font = nullptr;

		HWND m_window = NULL;
		ID3D12CommandQueue* m_command_queue = nullptr;
	private:
		ID3D12Device* m_d3d_device = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_rvt_handle;
		size_t m_buffer_count = -1;
		_FrameContext* m_frame_context = nullptr;
		ID3D12DescriptorHeap* m_descriptor_heap_backbuffer = nullptr;
		ID3D12DescriptorHeap* m_descriptor_heap_render = nullptr;
		ID3D12GraphicsCommandList* m_command_list = nullptr;
		std::map<std::uint32_t, dx_callback> m_dx_callbacks;
		std::vector<wndproc_callback> m_wndproc_callbacks;
	};

	inline renderer *g_renderer{};
}
