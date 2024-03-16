#include "player_service.hpp"

namespace big
{
    player_service::player_service()
    {
        g_player_service = this;
    }

    player_service::~player_service()
    {
        g_player_service = nullptr;
    }

    void player_service::get_player(LocalPlayer* local_player)
    {
        if (local_player == nullptr) return;

        this->m_player = std::make_shared<player>(local_player);
    }
}