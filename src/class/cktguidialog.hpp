#pragma once

#include <cstdint>

#pragma pack(push, 1)

class DetailCharacterInfo
{
public:
    char pad_0000[0x24]; //0x0000
    float m_health; //0x0024
    float m_mana; //0x0028
    float m_physical_attack; //0x002C
    float m_magical_attack; //0x0030
    char pad_0034[0x34]; //0x0034
    float m_awakening_duration; //0x0068
    float m_physical_defense; //0x006C
    float m_magical_defense; //0x0070
};

class CharacterInfo
{
public:
    class DetailCharacterInfo* m_detail_info; //0x0000
};

class CXUICharInfo
{
public:
    char pad_0000[0x350]; //0x0000
    class CharacterInfo* m_character_info; //0x0350
};

class STXWidgetWrapper
{
public:
    char pad_0000[0x260]; //0x0000
    class CXUICharInfo* m_cx_ui_char_info; //0x0260
};

class CKTDGUIDialog
{
public:
    char pad_0000[0xFC0]; //0x0000
    class STXWidgetWrapper* m_stx_widget_wrapper; //0x0FC0
};

static_assert(sizeof(DetailCharacterInfo) == 0x74);
static_assert(sizeof(CharacterInfo) == 0x8);
static_assert(sizeof(CXUICharInfo) == 0x358);
static_assert(sizeof(STXWidgetWrapper) == 0x268);
static_assert(sizeof(CKTDGUIDialog) == 0xFC8);

#pragma pack(pop)