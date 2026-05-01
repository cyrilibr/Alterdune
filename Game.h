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
using namespace std;

struct CombatResult {
    bool playerWon = false;
    bool playerDied = false;
    bool monsterSpared = false;
    BeastiaryEntry entry{};
};

string trim(const string& text);

class Game {
private:
    Player player;
    vector<unique_ptr<Monster>> monsterPool;
    map<string, ActDefinition> actCatalog;
    vector<BeastiaryEntry> beastiary;
    vector<string> journal;
    mt19937 rng;
    string savePath;

public:
    explicit Game(const string& playerName);

    bool loadData(const string& itemsPath, const string& monstersPath);
    void initializeNewPlayer(const string& playerName);
    void printStartSummary() const;
    void run();

    static int readChoice(int min, int max);

    bool saveGame() const;
    bool loadGame();
    bool hasSaveFile() const;

private:
    void buildActCatalog();
    bool loadItems(const string& path, vector<Item>& outItems) const;
    bool loadMonsters(const string& path, vector<unique_ptr<Monster>>& outMonsters);

    int randomDamage(int defenderMaxHp);
    void showBeastiary() const;
    static string categoryToString(MonsterCategory category);
    static bool parseCategory(const string& text, MonsterCategory& outCategory);
    static string itemTypeToString(ItemType type);
    static bool parseItemType(const string& text, ItemType& outType);
    void showInventoryMenu(bool outsideCombat);
    Monster& randomMonster();
    void startCombat();
    CombatResult runCombat(Monster& monster);
    void performAct(Monster& monster);
    void printEnding();

    void addJournal(const string& entry);
    void showJournal() const;
};
