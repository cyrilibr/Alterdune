#pragma once

#include <string>

#include "Monster.h"
using namespace std;

struct EntreeBestiaire {
    string nom;
    MonsterCategory categorie;
    int hpMax;
    int attaque;
    int defense;
    bool epargne;
};
