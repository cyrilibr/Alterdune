#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ActDefinition.h"
#include "Item.h"
#include "Monster.h"
using namespace std;

class CsvLoader {
public:
    static bool loadItems(const string& path, vector<Item>& outItems);
    static bool loadMonsters(const string& path,
                             const map<string, ActDefinition>& actCatalog,
                             vector<unique_ptr<Monster>>& outMonsters);
};
