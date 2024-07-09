#pragma once
#include "common.hpp"
#include "detour_hook.hpp"
#include "vmt_hook.hpp"

#include "class/abstract.hpp"

namespace big
{
	struct hooks
	{
		static void* convert_thread_to_fiber(void* param);

		static constexpr auto present_index = 140;
		static constexpr auto resizebuffer_index = 145;
		static constexpr auto execute_command_list = 54;
		static HRESULT APIENTRY swapchain_present(IDXGISwapChain3* _this, UINT SyncInterval, UINT Flags);
		static HRESULT APIENTRY swapchain_resizebuffers(IDXGISwapChain3* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);
		static HRESULT APIENTRY swapchain_execute_command_list(ID3D12CommandQueue* queue, UINT num_command_list, ID3D12CommandList* command_list);

		static LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static BOOL set_cursor_pos(int x, int y);
	};

	struct minhook_keepalive
	{
		minhook_keepalive();
		~minhook_keepalive();
	};

	class hooking
	{
		friend hooks;
	public:
		explicit hooking();
		~hooking();

		void enable();
		void disable();
	private:
		bool m_enabled{};
		minhook_keepalive m_minhook_keepalive;

		WNDPROC m_og_wndproc;

		detour_hook m_on_present;
		detour_hook m_resizebuffers;
		detour_hook m_command_queue;
		detour_hook m_set_cursor_pos_hook;
	};

	inline hooking* g_hooking{};
}
