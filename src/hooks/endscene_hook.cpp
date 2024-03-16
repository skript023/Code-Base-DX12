#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

namespace big
{
	HRESULT APIENTRY hooks::present(IDXGISwapChain3* _this, UINT SyncInterval, UINT Flags)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				if ((g_renderer->m_init || g_renderer->init(_this, SyncInterval, Flags)))
					g_renderer->on_present();
			}

			return g_hooking->m_end_scene_hook.get_original<decltype(&present)>()(_this, SyncInterval, Flags);
		} EXCEPT_CLAUSE

			return NULL;
	}
}