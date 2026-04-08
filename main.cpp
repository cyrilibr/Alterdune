#include <iostream>
#include <string>

#include "Game.h"
#include "UiTheme.h"

int main() {
    UiTheme::header("BIENVENUE DANS ALTERDUNE");
    std::cout << "Entrez le nom de votre personnage: ";

    std::string playerName;
    std::getline(std::cin, playerName);
    playerName = trim(playerName);

    if (playerName.empty()) {
        playerName = "Aventurier";
    }

    Game game(playerName);
    if (!game.loadData("items.csv", "monsters.csv")) {
        std::cerr << "Le jeu ne peut pas demarrer sans ses fichiers de donnees.\n";
        return 1;
    }

    game.printStartSummary();
    game.run();
    return 0;
}
