#include "imgui.h"
#include "player.h"
#include "script.hpp"
#include "settings.hpp"

#include "utility/els_util.hpp"
#include "services/player/player_service.hpp"

namespace big
{
    void player_menu::render_menu()
    {
        auto end = std::chrono::system_clock::now();
        std::time_t date = std::chrono::system_clock::to_time_t(end);

        ImGui::Text("%s", std::ctime(&date));

        auto is_character_selection = g_player_service->players().get() == nullptr;

        if (!is_character_selection)
            ImGui::Text("Health : %f/%f", g_player_service->players()->get_player_health(), els_util::detail_stats()->m_health);

        ImGui::BeginGroup();

        ImGui::Checkbox("Rapid Skill", &g_settings->self.rapid_skill);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Skill can be spammed rapidly.");

        ImGui::SameLine(150.f);

        ImGui::Checkbox("Godmode", &g_settings->self.godmode);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("This feature still have some buggy not tested well.");

        ImGui::SameLine(300.f);

        ImGui::Checkbox("Auto Kill", &g_settings->self.auto_kill);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("[WARNING] This feature may detected by server");

        ImGui::EndGroup();

        ImGui::BeginGroup();

        ImGui::Checkbox("Critical Damage", &g_settings->self.crit_dmg);
        if (g_settings->self.crit_dmg)
            ImGui::SliderFloat("Crit Damage", &g_settings->self.damage_crit, 0.f, 1000.f);

        ImGui::Checkbox("Critical Rate", &g_settings->self.crit_rate);
        if (g_settings->self.crit_dmg)
            ImGui::SliderFloat("Crit Rate", &g_settings->self.rate_crit, 0.f, 1000.f);

        ImGui::Checkbox("Attack Speed", &g_settings->self.atk_speed);
        if (g_settings->self.crit_dmg)
            ImGui::SliderFloat("Atk Speed", &g_settings->self.speed_atk, 0.f, 1000.f);

        ImGui::EndGroup();
    }
}