#include "common.hpp"
#include "class/abstract.hpp"

namespace big
{
    class player final
    {
        friend class player_service;

        LocalPlayer* m_local_player = nullptr;
    public:
        explicit player(LocalPlayer* m_local_player);
        ~player() = default;

        player(const player&)                = default;
		player(player&&) noexcept            = default;
		player& operator=(const player&)     = default;
		player& operator=(player&&) noexcept = default;

        [[nodiscard]] float get_player_health();
        [[nodiscard]] float get_player_health() const;
    };

    inline player* g_player{};
} // namespace big
