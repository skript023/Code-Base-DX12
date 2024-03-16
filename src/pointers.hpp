#pragma once
#include "common.hpp"
#include "class/vector.hpp"
#include "class/abstract.hpp"

namespace big
{
	class pointers
	{
	public:
		explicit pointers();
		~pointers();
	public:
		HWND m_hwnd{};
		uint64_t m_base_address;

		ScreenResolution* m_resolution;
		const char* m_character_name;
		void* m_return_address;
		CKTDGUIDialog** m_player_stats;
		void* m_rest_buff;
	public:
		void* m_swapchain_methods[150];
		void* m_device_methods[120];
		void* m_context_methods[145];
		IDXGISwapChain* m_swapchain;
	private:
		bool get_swapchain();
		bool swapchain_found = false;
		HWND m_window;
	};

	inline pointers* g_pointers{};
}
