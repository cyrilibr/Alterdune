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
    void definirNomInterne(const string& newName);
    void definirHpMaxInterne(int newMaxHp);
    void definirHpActuelsInterne(int newHp);

public:
    Character(string name, int maxHp, int attack, int defense);
    virtual ~Character();

    const string& getNom() const;
    int getHp() const;
    int getHpMax() const;
    int getAttaque() const;
    int getDefense() const;

    bool estVivant() const;

    void soigner(int amount);
    void recevoirDegats(int damage);
};
