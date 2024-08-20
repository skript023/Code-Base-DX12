#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

namespace big
{
    HRESULT APIENTRY hooks::swapchain_resizebuffers(IDXGISwapChain3* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		if (g_running)
		{
			g_renderer->pre_reset();

			auto result = g_hooking->m_resizebuffers.get_original<decltype(&swapchain_resizebuffers)>()(this_, buffer_count, width, height, new_format, swapchain_flags);

			if (SUCCEEDED(result))
			{
				g_renderer->post_reset(this_);
			}

			return result;
		}

		return g_hooking->m_resizebuffers.get_original<decltype(&swapchain_resizebuffers)>()(this_, buffer_count, width, height, new_format, swapchain_flags);
	}
} // namespace big
