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

public:
    Player(std::string name, int maxHp, int attack, int defense);

    int actCount() const override;

    std::vector<Item>& getInventory();
    const std::vector<Item>& getInventory() const;

    void setInventory(std::vector<Item> loadedItems);

    int getKills() const;
    int getSpared() const;
    int getVictories() const;

    void addKill();
    void addSpared();

    void printStats() const;
};
