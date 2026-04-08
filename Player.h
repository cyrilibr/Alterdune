#pragma once

#include <vector>

#include "Character.h"
#include "Item.h"

class Player : public Character {
private:
    std::vector<Item> inventory;
    int kills;
    int spared;
    int victories;
    int totalCombats;
    int totalDamageDealt;
    int totalDamageTaken;
    int totalHealingDone;

public:
    Player(std::string name, int maxHp, int attack, int defense);

    int actCount() const override;

    std::vector<Item>& getInventory();
    const std::vector<Item>& getInventory() const;

    void setInventory(std::vector<Item> loadedItems);

    int getKills() const;
    int getSpared() const;
    int getVictories() const;
    int getTotalCombats() const;
    int getTotalDamageDealt() const;
    int getTotalDamageTaken() const;
    int getTotalHealingDone() const;

    void setName(const std::string& newName);
    void setMaxHpAndClamp(int newMaxHp);
    void setCurrentHpClamped(int newHp);
    void setProgress(int newKills, int newSpared, int newVictories);
    void setAdvancedStats(int combats, int dealt, int taken, int healing);

    void addKill();
    void addSpared();
    void addCombat();
    void addDamageDealt(int value);
    void addDamageTaken(int value);
    void addHealingDone(int value);

    void printStats() const;
};
