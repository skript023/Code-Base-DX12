#include "player.hpp"

namespace big
{
    class player_service
    {
    public:
        player_service();
		~player_service();

		player_service(const player_service&)                = delete;
		player_service(player_service&&) noexcept            = delete;
		player_service& operator=(const player_service&)     = delete;
		player_service& operator=(player_service&&) noexcept = delete;

        void do_cleanup();
        void get_player(LocalPlayer* local_player);
        std::shared_ptr<player> players() { return m_player; }
    private:
        std::shared_ptr<player> m_player;
    };

    inline player_service* g_player_service;
} // namespace big
