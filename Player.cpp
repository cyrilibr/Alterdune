#include "Player.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "UiTheme.h"
using namespace std;

Player::Player(string name, int maxHp, int attack, int defense)
    : Character(move(name), maxHp, attack, defense),
      kills(0),
      spared(0),
      victories(0),
      totalCombats(0),
      totalDamageDealt(0),
      totalDamageTaken(0),
      totalHealingDone(0) {}

int Player::actCount() const { return 0; }

vector<Item>& Player::getInventory() { return inventory; }
const vector<Item>& Player::getInventory() const { return inventory; }

void Player::setInventory(vector<Item> loadedItems) { inventory = move(loadedItems); }

int Player::getKills() const { return kills; }
int Player::getSpared() const { return spared; }
int Player::getVictories() const { return victories; }
int Player::getTotalCombats() const { return totalCombats; }
int Player::getTotalDamageDealt() const { return totalDamageDealt; }
int Player::getTotalDamageTaken() const { return totalDamageTaken; }
int Player::getTotalHealingDone() const { return totalHealingDone; }

void Player::setName(const string& newName) { name = newName; }

void Player::setMaxHpAndClamp(int newMaxHp) {
    if (newMaxHp <= 0) return;
    maxHp = newMaxHp;
    hp = min(hp, maxHp);
}

void Player::setCurrentHpClamped(int newHp) { hp = max(0, min(newHp, maxHp)); }

void Player::setProgress(int newKills, int newSpared, int newVictories) {
    kills = max(0, newKills);
    spared = max(0, newSpared);
    victories = max(0, newVictories);
}

void Player::setAdvancedStats(int combats, int dealt, int taken, int healing) {
    totalCombats = max(0, combats);
    totalDamageDealt = max(0, dealt);
    totalDamageTaken = max(0, taken);
    totalHealingDone = max(0, healing);
}

void Player::addKill() {
    ++kills;
    ++victories;
}

void Player::addSpared() {
    ++spared;
    ++victories;
}

void Player::addCombat() { ++totalCombats; }

void Player::addDamageDealt(int value) {
    if (value > 0) totalDamageDealt += value;
}

void Player::addDamageTaken(int value) {
    if (value > 0) totalDamageTaken += value;
}

void Player::addHealingDone(int value) {
    if (value > 0) totalHealingDone += value;
}

void Player::printStats() const {
    UiTheme::header("STATISTIQUES DU JOUEUR");
    cout << "Nom: " << UiTheme::paint(name, UiTheme::soft) << "\n";
    cout << "HP: " << hp << "/" << maxHp << " " << UiTheme::gauge(hp, maxHp) << "\n";
    cout << "Monstres tues: " << kills << "\n";
    cout << "Monstres epargnes: " << spared << "\n";
    cout << "Victoires: " << victories << "/10\n";
    cout << UiTheme::paint(UiTheme::divider('-'), UiTheme::accent) << "\n";
    cout << "Combats totaux: " << totalCombats << "\n";
    cout << "Degats infliges: " << totalDamageDealt << "\n";
    cout << "Degats recus: " << totalDamageTaken << "\n";
    cout << "Soins utilises: " << totalHealingDone << "\n";

    int totalOutcomes = kills + spared;
    double spareRate = (totalOutcomes > 0) ? (100.0 * static_cast<double>(spared) / static_cast<double>(totalOutcomes)) : 0.0;
    cout << "Taux d'epargne: " << static_cast<int>(spareRate + 0.5) << "%\n";
    cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}
