#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

namespace big
{
	HRESULT APIENTRY hooks::swapchain_execute_command_list(ID3D12CommandQueue* queue, UINT num_command_list, ID3D12CommandList* command_list)
	{
		TRY_CLAUSE
		{
			if (!g_renderer->m_command_queue)
				g_renderer->m_command_queue = queue;

			return g_hooking->m_command_queue.get_original<decltype(&swapchain_execute_command_list)>()(queue, num_command_list, command_list);
		} EXCEPT_CLAUSE

			return NULL;
	}
}