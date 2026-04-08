#include "Player.h"

#include <iostream>
#include <utility>

Player::Player(std::string name, int maxHp, int attack, int defense)
    : Character(std::move(name), maxHp, attack, defense), kills(0), spared(0), victories(0) {}

int Player::actCount() const { return 0; }

std::vector<Item>& Player::getInventory() { return inventory; }
const std::vector<Item>& Player::getInventory() const { return inventory; }

void Player::setInventory(std::vector<Item> loadedItems) { inventory = std::move(loadedItems); }

int Player::getKills() const { return kills; }
int Player::getSpared() const { return spared; }
int Player::getVictories() const { return victories; }

void Player::addKill() {
    ++kills;
    ++victories;
}

void Player::addSpared() {
    ++spared;
    ++victories;
}

void Player::printStats() const {
    std::cout << "\n===== STATISTIQUES DU JOUEUR =====\n";
    std::cout << "Nom: " << name << "\n";
    std::cout << "HP: " << hp << "/" << maxHp << "\n";
    std::cout << "Monstres tues: " << kills << "\n";
    std::cout << "Monstres epargnes: " << spared << "\n";
    std::cout << "Victoires: " << victories << "/10\n";
    std::cout << "==================================\n";
}
