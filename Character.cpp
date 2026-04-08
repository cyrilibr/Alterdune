#include "Character.h"

#include <algorithm>
#include <utility>

Character::Character(std::string name, int maxHp, int attack, int defense)
    : name(std::move(name)), hp(maxHp), maxHp(maxHp), attack(attack), defense(defense) {}

Character::~Character() = default;

const std::string& Character::getName() const { return name; }
int Character::getHp() const { return hp; }
int Character::getMaxHp() const { return maxHp; }
int Character::getAttack() const { return attack; }
int Character::getDefense() const { return defense; }

bool Character::isAlive() const { return hp > 0; }

void Character::heal(int amount) {
    if (amount <= 0) return;
    hp = std::min(maxHp, hp + amount);
}

void Character::takeDamage(int damage) {
    if (damage <= 0) return;
    hp = std::max(0, hp - damage);
}
