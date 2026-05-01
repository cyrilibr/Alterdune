#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Character.h"
#include "Item.h"
using namespace std;

class Player : public Character {
private:
    vector<Item> inventory;
    int kills;
    int spared;
    int victories;
    int totalCombats;
    int totalDamageDealt;
    int totalDamageTaken;
    int totalHealingDone;

public:
    Player(string name, int maxHp, int attack, int defense);

    const vector<Item>& getInventaire() const;
    void definirInventaire(vector<Item> loadedItems);
    void ajouterItem(const Item& item);
    bool utiliserItem(size_t index, int& soinEffectue, string& itemName);
    bool aUnItemUtilisable() const;

    int getMonstresTues() const;
    int getMonstresEpargnes() const;
    int getVictoires() const;
    int getTotalCombats() const;
    int getTotalDamageDealt() const;
    int getTotalDamageTaken() const;
    int getTotalHealingDone() const;

    void definirNom(const string& newName);
    void definirHpMax(int newMaxHp);
    void definirHpActuels(int newHp);
    void definirProgression(int newKills, int newSpared, int newVictories);
    void definirStatistiquesAvancees(int combats, int dealt, int taken, int soin);

    void ajouterMonstreTue();
    void ajouterMonstreEpargne();
    void ajouterCombat();
    void ajouterDegatsInfliges(int value);
    void ajouterDegatsRecus(int value);
    void ajouterSoinEffectue(int value);

    void afficherStatistiques() const;
};
