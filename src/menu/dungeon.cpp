#include "dungeon.h"
#include "imgui.h"
#include "script.hpp"
#include "settings.hpp"

namespace big
{
	void dungeon::render_menu()
	{
        const char* const dungeons[] = { "Elrianode City", "El Tower Defense" };
		static int selected_dungeon = 0;

		if (ImGui::BeginCombo("Dungeon", dungeons[selected_dungeon]))
		{
			for (int i = 0; i < IM_ARRAYSIZE(dungeons); i++)
			{
				bool is_selected = selected_dungeon == i;

				if (ImGui::Selectable(dungeons[i], is_selected))
				{
					selected_dungeon = i;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
}