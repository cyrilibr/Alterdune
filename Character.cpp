#include "Character.h"

#include <algorithm>
#include <utility>
using namespace std;

Character::Character(string name, int maxHp, int attack, int defense)
    : name(move(name)), hp(maxHp), maxHp(maxHp), attack(attack), defense(defense) {}

Character::~Character() = default;

const string& Character::getName() const { return name; }
int Character::getHp() const { return hp; }
int Character::getMaxHp() const { return maxHp; }
int Character::getAttack() const { return attack; }
int Character::getDefense() const { return defense; }

bool Character::isAlive() const { return hp > 0; }

void Character::heal(int amount) {
    if (amount <= 0) return;
    hp = min(maxHp, hp + amount);
}

void Character::takeDamage(int damage) {
    if (damage <= 0) return;
    hp = max(0, hp - damage);
}

void Character::setNameInternal(const string& newName) { name = newName; }

void Character::setMaxHpInternal(int newMaxHp) {
    if (newMaxHp <= 0) return;
    maxHp = newMaxHp;
    hp = min(hp, maxHp);
}

void Character::setCurrentHpInternal(int newHp) {
    hp = max(0, min(newHp, maxHp));
}
