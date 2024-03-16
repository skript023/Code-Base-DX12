#pragma once
#include <cstdint>

#pragma pack(push, 1)
class LocalPlayer
{
public:
    char pad_0000[0x224];
    float m_health;
};
#pragma pack(pop)