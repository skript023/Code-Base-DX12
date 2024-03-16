#include "pointers.hpp"
#include "class/cktguidialog.hpp"

namespace big::els_util
{
    DetailCharacterInfo* detail_stats()
    {
        if (auto cktd_gui = *g_pointers->m_player_stats)
        {
            if (auto stx = cktd_gui->m_stx_widget_wrapper)
            {
                if (auto cx_char = stx->m_cx_ui_char_info)
                {
                    if (auto char_info = cx_char->m_character_info)
                    {
                        if (auto detail = char_info->m_detail_info)
                        {
                            return detail;
                        }
                    }
                }
            }
        }

        return nullptr;
    }
}