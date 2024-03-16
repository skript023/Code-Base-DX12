#include "hooking.hpp"
#include "class/local_player.hpp"
#include "services/player/player_service.hpp"

namespace big
{
    void __fastcall hooks::rest_buff(LocalPlayer* a1, float a2)
    {
		g_player_service->get_player(a1);

        return g_hooking->m_rest_buff.get_original<decltype(&hooks::rest_buff)>()(a1, a2);
    }
}
