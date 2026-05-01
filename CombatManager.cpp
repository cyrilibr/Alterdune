#include "CombatManager.h"

#include <iostream>
#include <limits>
#include <string>
using namespace std;

namespace {
int readChoice(int min, int max) {
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

int randomDamage(mt19937& rng, int defenderMaxHp) {
    uniform_int_distribution<int> distribution(0, defenderMaxHp);
    return distribution(rng);
}
}

CombatResult CombatManager::runCombat(Player& player,
                                      Monster& monster,
                                      const map<string, ActDefinition>& actCatalog,
                                      mt19937& rng) {
    CombatResult result;
    result.entry = {monster.getName(), monster.category(), monster.getMaxHp(), monster.getAttack(), monster.getDefense(), false};

    while (player.isAlive() && monster.isAlive()) {
        cout << "\n--- TOUR DU JOUEUR ---\n";
        cout << player.getName() << " HP: " << player.getHp() << "/" << player.getMaxHp() << "\n";
        cout << monster.getName() << " HP: " << monster.getHp() << "/" << monster.getMaxHp()
                  << " | Mercy: " << monster.getMercy() << "/" << monster.getMercyGoal() << "\n";
        cout << "1) FIGHT\n2) ACT\n3) ITEM\n4) MERCY\n";
        cout << "Choix: ";

        const int choice = readChoice(1, 4);

        if (choice == 1) {
            const int damage = randomDamage(rng, monster.getMaxHp());
            if (damage == 0) {
                cout << "Votre attaque rate !\n";
            } else {
                monster.takeDamage(damage);
                player.addDamageDealt(damage);
                cout << "Vous infligez " << damage << " degats a " << monster.getName() << ".\n";
            }

            if (!monster.isAlive()) {
                result.playerWon = true;
                result.monsterSpared = false;
                result.entry.spared = false;
                break;
            }
        } else if (choice == 2) {
            const auto& ids = monster.getActIds();
            cout << "Actions ACT disponibles:\n";
            for (size_t i = 0; i < ids.size(); ++i) {
                cout << i + 1 << ") " << ids[i] << "\n";
            }

            cout << "Choisissez une action: ";
            const int actChoice = readChoice(1, static_cast<int>(ids.size()));
            const string& actionId = ids[static_cast<size_t>(actChoice - 1)];
            const ActDefinition& action = actCatalog.at(actionId);

            const int mercyBefore = monster.getMercy();
            monster.adjustMercy(action.mercyImpact);
            const int mercyAfter = monster.getMercy();

            cout << action.text << "\n";
            cout << "Mercy: " << mercyBefore << " -> " << mercyAfter << "\n";
        } else if (choice == 3) {
            const auto& inventory = player.getInventory();
            if (inventory.empty()) {
                cout << "Aucun item disponible.\n";
            } else if (!player.hasUsableItem()) {
                cout << "Tous les items sont a quantite 0.\n";
            } else {
                cout << "Items:\n";
                for (size_t i = 0; i < inventory.size(); ++i) {
                    const Item& item = inventory[i];
                    cout << i + 1 << ") " << item.name << " x" << item.quantity
                         << " (soin " << item.value << ")\n";
                }

                cout << "0) Retour\n";
                cout << "Choisissez un item: ";
                const int itemChoice = readChoice(0, static_cast<int>(inventory.size()));

                if (itemChoice == 0) {
                    continue;
                }

                int healedAmount = 0;
                string itemName;

                if (!player.useItem(static_cast<size_t>(itemChoice - 1), healedAmount, itemName)) {
                    cout << "Item inutilisable (quantite 0 ou index invalide).\n";
                } else {
                    cout << itemName << " utilise. HP recuperes: " << healedAmount << "\n";
                    cout << "HP actuel: " << player.getHp() << "/" << player.getMaxHp() << "\n";
                }
            }
        } else {
            if (monster.canSpare()) {
                result.playerWon = true;
                result.monsterSpared = true;
                result.entry.spared = true;
                break;
            }

            cout << "Mercy insuffisante: " << monster.getMercy() << "/" << monster.getMercyGoal() << "\n";
        }

        if (!monster.isAlive() || result.monsterSpared) {
            break;
        }

        cout << "\n--- TOUR DU MONSTRE ---\n";
        const int damageToPlayer = randomDamage(rng, player.getMaxHp());

        if (damageToPlayer == 0) {
            cout << monster.getName() << " rate son attaque.\n";
        } else {
            player.takeDamage(damageToPlayer);
            player.addDamageTaken(damageToPlayer);
            cout << monster.getName() << " inflige " << damageToPlayer << " degats.\n";
        }

        if (!player.isAlive()) {
            result.playerDied = true;
            break;
        }
    }

    return result;
}
