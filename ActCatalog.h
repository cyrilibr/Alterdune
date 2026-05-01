#pragma once

#include <map>
#include <string>

#include "ActDefinition.h"
using namespace std;

class ActCatalog {
public:
    static map<string, ActDefinition> build();
};
