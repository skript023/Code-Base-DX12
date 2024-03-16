#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

namespace big
{
    HRESULT APIENTRY hooks::on_reset(IDXGISwapChain3* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				g_renderer->pre_reset();

				auto result = g_hooking->m_on_reset_hook.get_original<decltype(&on_reset)>()(this_, buffer_count, width, height, new_format, swapchain_flags);

				if (SUCCEEDED(result))
				{
					g_renderer->post_reset();
				}

				return result;
			}

			return g_hooking->m_on_reset_hook.get_original<decltype(&on_reset)>()(this_, buffer_count, width, height, new_format, swapchain_flags);
		} EXCEPT_CLAUSE

			return NULL;
	}
} // namespace big
