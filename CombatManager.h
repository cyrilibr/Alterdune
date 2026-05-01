#pragma once

#include <map>
#include <random>

#include "ActDefinition.h"
#include "BestiaryEntry.h"
#include "Monster.h"
#include "Player.h"
using namespace std;

struct ResultatCombat {
    bool joueurAGagne = false;
    bool joueurEstMort = false;
    bool monstreEpargne = false;
    EntreeBestiaire entree{};
};

class CombatManager {
public:
    static ResultatCombat runCombat(Player& joueur,
                                  Monster& monstre,
                                  const map<string, ActDefinition>& catalogueAct,
                                  mt19937& rng);
};
