#include <iostream>
#include <string>

#include "Game.h"
#include "UiTheme.h"
using namespace std;

int main() {
    UiTheme::printTitleScreen();

    Game game("Aventurier");
    if (!game.loadData("items.csv", "monsters.csv")) {
        cerr << "Le jeu ne peut pas demarrer sans ses fichiers de donnees.\n";
        return 1;
    }

    cout << "Entrez le nom de votre personnage: ";
    string playerName;
    getline(cin, playerName);

    game.initializeNewPlayer(playerName);
    game.printStartSummary();
    game.run();

    return 0;
}
