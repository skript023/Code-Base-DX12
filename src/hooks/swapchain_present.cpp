#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

namespace big
{
	HRESULT APIENTRY hooks::swapchain_present(IDXGISwapChain3* _this, UINT SyncInterval, UINT Flags)
	{
		if (g_running)
		{
			if (g_renderer.m_init || g_renderer.init(_this))
			{
				g_renderer.on_present(_this);
			}
		}

		return g_hooking->m_on_present.get_original<decltype(&swapchain_present)>()(_this, SyncInterval, Flags);
	}
}