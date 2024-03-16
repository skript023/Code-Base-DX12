#include "player.hpp"
#include "class/local_player.hpp"

namespace big
{
    player::player(LocalPlayer* m_local_player) : 
        m_local_player(m_local_player)
    {
        
    }

    float player::get_player_health()
    {
        if (this->m_local_player == nullptr) return 0.f;

        return this->m_local_player->m_health;
    }

    float player::get_player_health() const
    {
        if (this->m_local_player == nullptr) return 0.f;

        return this->m_local_player->m_health;
    }
}