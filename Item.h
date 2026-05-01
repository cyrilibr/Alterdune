#pragma once

#include <string>
using namespace std;

enum class ItemType { HEAL };

struct Item {
    string name;
    ItemType type;
    int value;
    int quantity;
};
