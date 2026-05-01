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

const vector<Item>& Player::getInventaire() const {
    return inventory;
}

void Player::definirInventaire(vector<Item> loadedItems) {
    inventory = move(loadedItems);
}

void Player::ajouterItem(const Item& item) {
    inventory.push_back(item);
}

bool Player::utiliserItem(size_t index, int& soinEffectue, string& itemName) {
    soinEffectue = 0;
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
    soigner(selected.value);
    soinEffectue = getHp() - hpBefore;

    --selected.quantity;
    ajouterSoinEffectue(soinEffectue);
    return true;
}

bool Player::aUnItemUtilisable() const {
    for (const Item& item : inventory) {
        if (item.quantity > 0) {
            return true;
        }
    }
    return false;
}

int Player::getMonstresTues() const {
    return kills;
}

int Player::getMonstresEpargnes() const {
    return spared;
}

int Player::getVictoires() const {
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

void Player::definirNom(const string& newName) {
    definirNomInterne(newName);
}

void Player::definirHpMax(int newMaxHp) {
    definirHpMaxInterne(newMaxHp);
}

void Player::definirHpActuels(int newHp) {
    definirHpActuelsInterne(newHp);
}

void Player::definirProgression(int newKills, int newSpared, int newVictories) {
    kills = max(0, newKills);
    spared = max(0, newSpared);
    victories = max(0, newVictories);
}

void Player::definirStatistiquesAvancees(int combats, int dealt, int taken, int soin) {
    totalCombats = max(0, combats);
    totalDamageDealt = max(0, dealt);
    totalDamageTaken = max(0, taken);
    totalHealingDone = max(0, soin);
}

void Player::ajouterMonstreTue() {
    ++kills;
    ++victories;
}

void Player::ajouterMonstreEpargne() {
    ++spared;
    ++victories;
}

void Player::ajouterCombat() {
    ++totalCombats;
}

void Player::ajouterDegatsInfliges(int value) {
    if (value > 0) {
        totalDamageDealt += value;
    }
}

void Player::ajouterDegatsRecus(int value) {
    if (value > 0) {
        totalDamageTaken += value;
    }
}

void Player::ajouterSoinEffectue(int value) {
    if (value > 0) {
        totalHealingDone += value;
    }
}

void Player::afficherStatistiques() const {
    UiTheme::header("STATISTIQUES DU JOUEUR");
    cout << "Nom: " << UiTheme::paint(getNom(), UiTheme::soft) << "\n";
    cout << "HP: " << getHp() << "/" << getHpMax() << " "
         << UiTheme::gauge(getHp(), getHpMax()) << "\n";
    cout << "Monstres tues: " << kills << "\n";
    cout << "Monstres epargnes: " << spared << "\n";
    cout << "Victoires: " << victories << "/10\n";
}
