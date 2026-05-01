#include "Character.h"

#include <algorithm>
#include <utility>
using namespace std;

Character::Character(string name, int maxHp, int attack, int defense)
    : name(move(name)), hp(maxHp), maxHp(maxHp), attack(attack), defense(defense) {}

Character::~Character() = default;

const string& Character::getNom() const { return name; }
int Character::getHp() const { return hp; }
int Character::getHpMax() const { return maxHp; }
int Character::getAttaque() const { return attack; }
int Character::getDefense() const { return defense; }

bool Character::estVivant() const { return hp > 0; }

void Character::soigner(int amount) {
    if (amount <= 0) return;
    hp = min(maxHp, hp + amount);
}

void Character::recevoirDegats(int damage) {
    if (damage <= 0) return;
    hp = max(0, hp - damage);
}

void Character::definirNomInterne(const string& newName) { name = newName; }

void Character::definirHpMaxInterne(int newMaxHp) {
    if (newMaxHp <= 0) return;
    maxHp = newMaxHp;
    hp = min(hp, maxHp);
}

void Character::definirHpActuelsInterne(int newHp) {
    hp = max(0, min(newHp, maxHp));
}
