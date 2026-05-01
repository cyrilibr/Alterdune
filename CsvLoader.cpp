#include "CsvLoader.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

namespace {
string trim(const string& text) {
    size_t start = 0;
    while (start < text.size() && isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    size_t end = text.size();
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    return text.substr(start, end - start);
}

string toUpper(string text) {
    for (char& c : text) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return text;
}

vector<string> splitCsvLine(const string& line) {
    vector<string> columns;
    stringstream stream(line);
    string part;

    while (getline(stream, part, ';')) {
        columns.push_back(trim(part));
    }

    return columns;
}

bool parseInt(const string& text, int& value) {
    try {
        size_t pos = 0;
        int parsed = stoi(text, &pos);
        if (pos != text.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool isHeaderLine(const vector<string>& columns) {
    if (columns.size() < 2) {
        return false;
    }

    const string first = toUpper(columns[0]);
    const string second = toUpper(columns[1]);

    return (first == "NOM" && second == "TYPE") || (first == "CATEGORIE" && second == "NOM");
}
}

bool CsvLoader::loadItems(const string& path, vector<Item>& outItems) {
    ifstream file(path);
    if (!file) {
        cerr << "Erreur: fichier introuvable: " << path << "\n";
        return false;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        const vector<string> columns = splitCsvLine(line);
        if (columns.empty() || (columns.size() == 1 && columns[0].empty())) {
            continue;
        }

        if (isHeaderLine(columns)) {
            continue;
        }

        if (columns.size() != 4) {
            cerr << "items.csv ligne " << lineNumber << " invalide: format attendu nom;type;valeur;quantite\n";
            continue;
        }

        int value = 0;
        int quantity = 0;
        const string type = toUpper(columns[1]);

        if (columns[0].empty() || type != "HEAL" || !parseInt(columns[2], value) || !parseInt(columns[3], quantity) ||
            value <= 0 || quantity < 0) {
            cerr << "items.csv ligne " << lineNumber << " invalide: donnees incorrectes\n";
            continue;
        }

        outItems.push_back({columns[0], ItemType::HEAL, value, quantity});
    }

    return true;
}

bool CsvLoader::loadMonsters(const string& path,
                             const map<string, ActDefinition>& actCatalog,
                             vector<unique_ptr<Monster>>& outMonsters) {
    ifstream file(path);
    if (!file) {
        cerr << "Erreur: fichier introuvable: " << path << "\n";
        return false;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        const vector<string> columns = splitCsvLine(line);
        if (columns.empty() || (columns.size() == 1 && columns[0].empty())) {
            continue;
        }

        if (isHeaderLine(columns)) {
            continue;
        }

        if (columns.size() != 10) {
            cerr << "monsters.csv ligne " << lineNumber << " invalide: 10 colonnes attendues\n";
            continue;
        }

        const string categoryText = toUpper(columns[0]);
        const string name = columns[1];

        int hp = 0;
        int attack = 0;
        int defense = 0;
        int mercyGoal = 0;

        if (name.empty() || !parseInt(columns[2], hp) || !parseInt(columns[3], attack) ||
            !parseInt(columns[4], defense) || !parseInt(columns[5], mercyGoal) || hp <= 0 || attack < 0 ||
            defense < 0 || mercyGoal <= 0) {
            cerr << "monsters.csv ligne " << lineNumber << " invalide: stats ou nom incorrect\n";
            continue;
        }

        size_t expectedActs = 0;
        if (categoryText == "NORMAL") {
            expectedActs = 2;
        } else if (categoryText == "MINIBOSS") {
            expectedActs = 3;
        } else if (categoryText == "BOSS") {
            expectedActs = 4;
        } else {
            cerr << "monsters.csv ligne " << lineNumber << " invalide: categorie inconnue\n";
            continue;
        }

        vector<string> acts;
        bool hasUnknownAct = false;

        for (size_t i = 6; i < columns.size(); ++i) {
            const string actId = toUpper(columns[i]);
            if (actId.empty() || actId == "-") {
                continue;
            }

            if (actCatalog.find(actId) == actCatalog.end()) {
                cerr << "monsters.csv ligne " << lineNumber << " invalide: action inconnue " << actId << "\n";
                hasUnknownAct = true;
                break;
            }

            acts.push_back(actId);
        }

        if (hasUnknownAct) {
            continue;
        }

        if (acts.size() < expectedActs) {
            cerr << "monsters.csv ligne " << lineNumber << " invalide: pas assez d'actions ACT\n";
            continue;
        }

        acts.resize(expectedActs);

        unique_ptr<Monster> monster;
        if (categoryText == "NORMAL") {
            monster = make_unique<NormalMonster>(name, hp, attack, defense, mercyGoal, acts);
        } else if (categoryText == "MINIBOSS") {
            monster = make_unique<MiniBossMonster>(name, hp, attack, defense, mercyGoal, acts);
        } else {
            monster = make_unique<BossMonster>(name, hp, attack, defense, mercyGoal, acts);
        }

        outMonsters.push_back(move(monster));
    }

    if (outMonsters.empty()) {
        cerr << "Erreur: aucun monstre valide n'a ete charge depuis monsters.csv\n";
        return false;
    }

    return true;
}
