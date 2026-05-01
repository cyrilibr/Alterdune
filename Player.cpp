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

const vector<Item>& Player::getInventory() const {
    return inventory;
}

void Player::setInventory(vector<Item> loadedItems) {
    inventory = move(loadedItems);
}

void Player::addItem(const Item& item) {
    inventory.push_back(item);
}

bool Player::useItem(size_t index, int& healedAmount, string& itemName) {
    healedAmount = 0;
    itemName.clear();

    if (index >= inventory.size()) {
        return false;
    }

    Item& selected = inventory[index];
    if (selected.quantity <= 0) {
        return false;
    }

    itemName = selected.name;
    int hpBefore = getHp();
    heal(selected.value);
    healedAmount = getHp() - hpBefore;

    --selected.quantity;
    addHealingDone(healedAmount);
    return true;
}

bool Player::hasUsableItem() const {
    for (const Item& item : inventory) {
        if (item.quantity > 0) {
            return true;
        }
    }
    return false;
}

int Player::getKills() const {
    return kills;
}

int Player::getSpared() const {
    return spared;
}

int Player::getVictories() const {
    return victories;
}

int Player::getTotalCombats() const {
    return totalCombats;
}

int Player::getTotalDamageDealt() const {
    return totalDamageDealt;
}

int Player::getTotalDamageTaken() const {
    return totalDamageTaken;
}

int Player::getTotalHealingDone() const {
    return totalHealingDone;
}

void Player::setName(const string& newName) {
    setNameInternal(newName);
}

void Player::setMaxHpAndClamp(int newMaxHp) {
    setMaxHpInternal(newMaxHp);
}

void Player::setCurrentHpClamped(int newHp) {
    setCurrentHpInternal(newHp);
}

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

void Player::addCombat() {
    ++totalCombats;
}

void Player::addDamageDealt(int value) {
    if (value > 0) {
        totalDamageDealt += value;
    }
}

void Player::addDamageTaken(int value) {
    if (value > 0) {
        totalDamageTaken += value;
    }
}

void Player::addHealingDone(int value) {
    if (value > 0) {
        totalHealingDone += value;
    }
}

void Player::printStats() const {
    UiTheme::header("STATISTIQUES DU JOUEUR");
    cout << "Nom: " << UiTheme::paint(getName(), UiTheme::soft) << "\n";
    cout << "HP: " << getHp() << "/" << getMaxHp() << " "
         << UiTheme::gauge(getHp(), getMaxHp()) << "\n";
    cout << "Monstres tues: " << kills << "\n";
    cout << "Monstres epargnes: " << spared << "\n";
    cout << "Victoires: " << victories << "/10\n";
}
