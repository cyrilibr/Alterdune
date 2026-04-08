#pragma once

#include <string>

class Character {
protected:
    std::string name;
    int hp;
    int maxHp;
    int attack;
    int defense;

public:
    Character(std::string name, int maxHp, int attack, int defense);
    virtual ~Character();

    const std::string& getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;

    bool isAlive() const;

    void heal(int amount);
    void takeDamage(int damage);

    virtual int actCount() const = 0;
};
