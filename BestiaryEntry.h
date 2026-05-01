#pragma once

#include <string>

#include "Monster.h"
using namespace std;

struct BestiaryEntry {
    string name;
    MonsterCategory category;
    int maxHp;
    int attack;
    int defense;
    bool spared;
};
