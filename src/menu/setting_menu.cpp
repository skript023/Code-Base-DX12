#include "imgui.h"
#include "script.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "setting_menu.h"

namespace big
{
    void setting_menu::render_menu()
    {
        if (ImGui::Button("Quit"))
        {
            g_running = false;
        }
    }

    void setting_menu::gui_settings()
    {
		ImGui::Text("UI Scale");
		if (ImGui::SliderFloat("##gui-scale", &g_settings->window.gui_scale, 0.75f, 1.5f, "%.2f"))
			g_renderer.rescale(g_settings->window.gui_scale);

		ImGui::Text("UI Color");
		static ImVec4 col_gui = ImGui::ColorConvertU32ToFloat4(g_settings->window.background_color);
		if (ImGui::ColorEdit4("Pick UI Color", (float*)&col_gui, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g_settings->window.background_color = ImGui::ColorConvertFloat4ToU32(col_gui);
		}

		static ImVec4 col_text = ImGui::ColorConvertU32ToFloat4(g_settings->window.text_color);
		if (ImGui::ColorEdit4("Text Color", (float*)&col_text, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g_settings->window.text_color = ImGui::ColorConvertFloat4ToU32(col_text);
		}

		static ImVec4 col_button = ImGui::ColorConvertU32ToFloat4(g_settings->window.button_color);
		if (ImGui::ColorEdit4("Button Color", (float*)&col_button, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g_settings->window.button_color = ImGui::ColorConvertFloat4ToU32(col_button);
		}

		static ImVec4 col_frame = ImGui::ColorConvertU32ToFloat4(g_settings->window.frame_color);
		if (ImGui::ColorEdit4("Frame Color", (float*)&col_frame, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoSidePreview))
		{
			g_settings->window.frame_color = ImGui::ColorConvertFloat4ToU32(col_frame);
		}
    }
}