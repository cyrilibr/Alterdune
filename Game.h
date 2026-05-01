#pragma once

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "ActDefinition.h"
#include "BestiaryEntry.h"
#include "Monster.h"
#include "Player.h"
using namespace std;

class Game {
public:
    explicit Game(const string& playerName);

    bool loadData(const string& itemsPath, const string& monstersPath);
    void initializeNewPlayer(const string& playerName);
    void printStartSummary() const;
    void run();

    static int readChoice(int min, int max);

private:
    Player player;
    vector<unique_ptr<Monster>> monsterPool;
    map<string, ActDefinition> actCatalog;
    vector<BestiaryEntry> bestiary;
    mt19937 rng;

    void showBestiary() const;
    void showInventoryMenu(bool outsideCombat);
    Monster& randomMonster();
    void startCombat();
    void printEnding() const;
    static string categoryToString(MonsterCategory category);
};
