#include "Game.h"

#include <iostream>
#include <limits>

#include "ActCatalog.h"
#include "CombatManager.h"
#include "CsvLoader.h"
using namespace std;

Game::Game(const string& playerName)
    : player(playerName, 120, 12, 8),
      actCatalog(ActCatalog::build()),
      rng(random_device{}()) {}

bool Game::loadData(const string& itemsPath, const string& monstersPath) {
    vector<Item> items;
    if (!CsvLoader::loadItems(itemsPath, items)) {
        return false;
    }

    vector<unique_ptr<Monster>> monsters;
    if (!CsvLoader::loadMonsters(monstersPath, actCatalog, monsters)) {
        return false;
    }

    player.setInventory(move(items));
    monsterPool = move(monsters);
    return true;
}

void Game::initializeNewPlayer(const string& playerName) {
    if (playerName.empty()) {
        player.setName("Aventurier");
    } else {
        player.setName(playerName);
    }

    player.setMaxHpAndClamp(120);
    player.setCurrentHpClamped(120);
    player.setProgress(0, 0, 0);
    player.setAdvancedStats(0, 0, 0, 0);
    bestiary.clear();
}

void Game::printStartSummary() const {
    cout << "\n===== RESUME DE LA PARTIE =====\n";
    cout << "Nom du joueur: " << player.getName() << "\n";
    cout << "HP: " << player.getHp() << "/" << player.getMaxHp() << "\n";
    cout << "Inventaire initial:\n";

    const auto& inventory = player.getInventory();
    if (inventory.empty()) {
        cout << "- (aucun item)\n";
    } else {
        for (const Item& item : inventory) {
            cout << "- " << item.name << " x" << item.quantity << "\n";
        }
    }

    cout << "===============================\n\n";
}

int Game::readChoice(int min, int max) {
    while (true) {
        int choice = 0;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entree invalide. Reessayez: ";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice < min || choice > max) {
            cout << "Choix hors limites. Reessayez: ";
            continue;
        }

        return choice;
    }
}

void Game::run() {
    bool quit = false;

    while (!quit) {
        if (player.getVictories() >= 10) {
            printEnding();
            break;
        }

        cout << "\n===== MENU PRINCIPAL =====\n";
        cout << "1. Bestiaire\n";
        cout << "2. Demarrer un combat\n";
        cout << "3. Statistiques du personnage\n";
        cout << "4. Items\n";
        cout << "5. Quitter\n";
        cout << "Votre choix: ";

        const int choice = readChoice(1, 5);

        if (choice == 1) {
            showBestiary();
        } else if (choice == 2) {
            startCombat();
        } else if (choice == 3) {
            player.printStats();
        } else if (choice == 4) {
            showInventoryMenu(true);
        } else {
            quit = true;
        }

        if (!player.isAlive()) {
            cout << "Vous etes tombe au combat. Partie perdue.\n";
            quit = true;
        }
    }
}

void Game::showBestiary() const {
    cout << "\n===== BESTIAIRE =====\n";

    if (bestiary.empty()) {
        cout << "Aucun monstre vaincu pour le moment.\n";
        return;
    }

    for (const BestiaryEntry& entry : bestiary) {
        cout << "Nom: " << entry.name << "\n";
        cout << "Categorie: " << categoryToString(entry.category) << "\n";
        cout << "HP max: " << entry.maxHp << " | ATK: " << entry.attack << " | DEF: " << entry.defense << "\n";
        cout << "Resultat: " << (entry.spared ? "Epargne" : "Tue") << "\n";
        cout << "-----------------------------\n";
    }
}

void Game::showInventoryMenu(bool outsideCombat) {
    if (!outsideCombat) {
        return;
    }

    while (true) {
        const auto& inventory = player.getInventory();

        cout << "\n===== ITEMS =====\n";
        if (inventory.empty()) {
            cout << "Inventaire vide.\n";
            return;
        }

        for (size_t i = 0; i < inventory.size(); ++i) {
            const Item& item = inventory[i];
            cout << i + 1 << ". " << item.name << " x" << item.quantity
                      << " (soigne " << item.value << " HP)\n";
        }

        cout << "0. Retour\n";
        cout << "Choisissez un item: ";

        const int choice = readChoice(0, static_cast<int>(inventory.size()));
        if (choice == 0) {
            return;
        }

        int healedAmount = 0;
        string itemName;

        if (!player.useItem(static_cast<size_t>(choice - 1), healedAmount, itemName)) {
            cout << "Cet item ne peut pas etre utilise (quantite a 0).\n";
            continue;
        }

        cout << itemName << " utilise. HP recuperes: " << healedAmount << "\n";
        cout << "HP actuel: " << player.getHp() << "/" << player.getMaxHp() << "\n";
    }
}

Monster& Game::randomMonster() {
    uniform_int_distribution<size_t> distribution(0, monsterPool.size() - 1);
    return *monsterPool[distribution(rng)];
}

void Game::startCombat() {
    player.addCombat();

    Monster& templateMonster = randomMonster();
    unique_ptr<Monster> monster = templateMonster.clone();

    cout << "\n===== DEBUT DU COMBAT =====\n";
    cout << "Un " << monster->categoryName() << " apparait: " << monster->getName() << "\n";

    CombatResult result = CombatManager::runCombat(player, *monster, actCatalog, rng);

    if (result.playerWon) {
        bestiary.push_back(result.entry);

        if (result.monsterSpared) {
            player.addSpared();
            cout << "Victoire pacifique ! " << monster->getName() << " est epargne.\n";
        } else {
            player.addKill();
            cout << "Victoire ! " << monster->getName() << " est vaincu.\n";
        }

        cout << "Victoires: " << player.getVictories() << "/10\n";
    } else if (result.playerDied) {
        cout << "Defaite immediate: " << player.getName() << " est tombe a 0 HP.\n";
    }
}

void Game::printEnding() const {
    cout << "\n===== FIN DE PARTIE =====\n";

    if (player.getKills() > 0 && player.getSpared() == 0) {
        cout << "FIN GENOCIDAIRE\n";
    } else if (player.getSpared() > 0 && player.getKills() == 0) {
        cout << "FIN PACIFISTE\n";
    } else {
        cout << "FIN NEUTRE\n";
    }

    cout << "Bilan final: tues=" << player.getKills()
              << ", epargnes=" << player.getSpared()
              << ", victoires=" << player.getVictories() << "/10\n";
}

string Game::categoryToString(MonsterCategory category) {
    if (category == MonsterCategory::NORMAL) {
        return "NORMAL";
    }
    if (category == MonsterCategory::MINIBOSS) {
        return "MINIBOSS";
    }
    return "BOSS";
}
