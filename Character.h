#pragma once

#include <string>
using namespace std;

class Character {
private:
    string name;
    int hp;
    int maxHp;
    int attack;
    int defense;

protected:
    void setNameInternal(const string& newName);
    void setMaxHpInternal(int newMaxHp);
    void setCurrentHpInternal(int newHp);

public:
    Character(string name, int maxHp, int attack, int defense);
    virtual ~Character();

    const string& getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttack() const;
    int getDefense() const;

    bool isAlive() const;

    void heal(int amount);
    void takeDamage(int damage);
};
