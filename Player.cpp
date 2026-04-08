#include "Player.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "UiTheme.h"

Player::Player(std::string name, int maxHp, int attack, int defense)
    : Character(std::move(name), maxHp, attack, defense),
      kills(0),
      spared(0),
      victories(0),
      totalCombats(0),
      totalDamageDealt(0),
      totalDamageTaken(0),
      totalHealingDone(0) {}

int Player::actCount() const { return 0; }

std::vector<Item>& Player::getInventory() { return inventory; }
const std::vector<Item>& Player::getInventory() const { return inventory; }

void Player::setInventory(std::vector<Item> loadedItems) { inventory = std::move(loadedItems); }

int Player::getKills() const { return kills; }
int Player::getSpared() const { return spared; }
int Player::getVictories() const { return victories; }
int Player::getTotalCombats() const { return totalCombats; }
int Player::getTotalDamageDealt() const { return totalDamageDealt; }
int Player::getTotalDamageTaken() const { return totalDamageTaken; }
int Player::getTotalHealingDone() const { return totalHealingDone; }

void Player::setName(const std::string& newName) { name = newName; }

void Player::setMaxHpAndClamp(int newMaxHp) {
    if (newMaxHp <= 0) return;
    maxHp = newMaxHp;
    hp = std::min(hp, maxHp);
}

void Player::setCurrentHpClamped(int newHp) { hp = std::max(0, std::min(newHp, maxHp)); }

void Player::setProgress(int newKills, int newSpared, int newVictories) {
    kills = std::max(0, newKills);
    spared = std::max(0, newSpared);
    victories = std::max(0, newVictories);
}

void Player::setAdvancedStats(int combats, int dealt, int taken, int healing) {
    totalCombats = std::max(0, combats);
    totalDamageDealt = std::max(0, dealt);
    totalDamageTaken = std::max(0, taken);
    totalHealingDone = std::max(0, healing);
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
    std::cout << "Nom: " << UiTheme::paint(name, UiTheme::soft) << "\n";
    std::cout << "HP: " << hp << "/" << maxHp << " " << UiTheme::gauge(hp, maxHp) << "\n";
    std::cout << "Monstres tues: " << kills << "\n";
    std::cout << "Monstres epargnes: " << spared << "\n";
    std::cout << "Victoires: " << victories << "/10\n";
    std::cout << UiTheme::paint(UiTheme::divider('-'), UiTheme::accent) << "\n";
    std::cout << "Combats totaux: " << totalCombats << "\n";
    std::cout << "Degats infliges: " << totalDamageDealt << "\n";
    std::cout << "Degats recus: " << totalDamageTaken << "\n";
    std::cout << "Soins utilises: " << totalHealingDone << "\n";

    int totalOutcomes = kills + spared;
    double spareRate = (totalOutcomes > 0) ? (100.0 * static_cast<double>(spared) / static_cast<double>(totalOutcomes)) : 0.0;
    std::cout << "Taux d'epargne: " << static_cast<int>(spareRate + 0.5) << "%\n";
    std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}
