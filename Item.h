#pragma once

#include <string>

enum class ItemType { HEAL };

struct Item {
    std::string name;
    ItemType type;
    int value;
    int quantity;
};
