#pragma once

#include <string>

#include "Monster.h"

struct BeastiaryEntry {
    std::string name;
    MonsterCategory category;
    int maxHp;
    int attack;
    int defense;
    bool spared;
};
