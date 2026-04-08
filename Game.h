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
    std::vector<std::string> journal;
    std::mt19937 rng;
    std::string savePath;

public:
    explicit Game(const std::string& playerName);

    bool loadData(const std::string& itemsPath, const std::string& monstersPath);
    void initializeNewPlayer(const std::string& playerName);
    void printStartSummary() const;
    void run();

    static int readChoice(int min, int max);

    bool saveGame() const;
    bool loadGame();
    bool hasSaveFile() const;

private:
    void buildActCatalog();
    bool loadItems(const std::string& path, std::vector<Item>& outItems) const;
    bool loadMonsters(const std::string& path, std::vector<std::unique_ptr<Monster>>& outMonsters);

    int randomDamage(int defenderMaxHp);
    void showBeastiary() const;
    static std::string categoryToString(MonsterCategory category);
    static bool parseCategory(const std::string& text, MonsterCategory& outCategory);
    static std::string itemTypeToString(ItemType type);
    static bool parseItemType(const std::string& text, ItemType& outType);
    void showInventoryMenu(bool outsideCombat);
    Monster& randomMonster();
    void startCombat();
    CombatResult runCombat(Monster& monster);
    void performAct(Monster& monster);
    void printEnding();

    void addJournal(const std::string& entry);
    void showJournal() const;
};
