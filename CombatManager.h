#pragma once

#include <map>
#include <random>

#include "ActDefinition.h"
#include "BestiaryEntry.h"
#include "Monster.h"
#include "Player.h"
using namespace std;

struct CombatResult {
    bool playerWon = false;
    bool playerDied = false;
    bool monsterSpared = false;
    BestiaryEntry entry{};
};

class CombatManager {
public:
    static CombatResult runCombat(Player& player,
                                  Monster& monster,
                                  const map<string, ActDefinition>& actCatalog,
                                  mt19937& rng);
};
