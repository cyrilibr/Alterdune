#pragma once

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "ActDefinition.h"
#include "BeastiaryEntry.h"
#include "Item.h"
#include "Monster.h"
#include "Player.h"

struct CombatResult {
    bool playerWon = false;
    bool playerDied = false;
    bool monsterSpared = false;
    BeastiaryEntry entry{};
};

std::string trim(const std::string& text);

class Game {
private:
    Player player;
    std::vector<std::unique_ptr<Monster>> monsterPool;
    std::map<std::string, ActDefinition> actCatalog;
    std::vector<BeastiaryEntry> beastiary;
    std::mt19937 rng;

public:
    explicit Game(const std::string& playerName);

    bool loadData(const std::string& itemsPath, const std::string& monstersPath);
    void printStartSummary() const;
    void run();

private:
    void buildActCatalog();
    bool loadItems(const std::string& path, std::vector<Item>& outItems) const;
    bool loadMonsters(const std::string& path, std::vector<std::unique_ptr<Monster>>& outMonsters);

    static int readChoice(int min, int max);
    int randomDamage(int defenderMaxHp);
    void showBeastiary() const;
    static std::string categoryToString(MonsterCategory category);
    void showInventoryMenu(bool outsideCombat);
    Monster& randomMonster();
    void startCombat();
    CombatResult runCombat(Monster& monster);
    void performAct(Monster& monster);
    void printEnding() const;
};
