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

    UiTheme::printMenuBlock("DEMARRAGE", {"1) Nouvelle partie", "2) Charger la sauvegarde", "3) Quitter"});
    cout << UiTheme::paint(" Votre choix > ", UiTheme::accent);
    int bootChoice = Game::readChoice(1, 3);

    if (bootChoice == 3) {
        cout << "A bientot dans ALTERDUNE.\n";
        return 0;
    }

    if (bootChoice == 2) {
        if (!game.loadGame()) {
            cout << UiTheme::paint("Aucune sauvegarde valide. Creation d'une nouvelle partie.", UiTheme::warn) << "\n";
            cout << "Entrez le nom de votre personnage: ";
            string playerName;
            getline(cin, playerName);
            game.initializeNewPlayer(playerName);
        }
    } else {
        cout << "Entrez le nom de votre personnage: ";
        string playerName;
        getline(cin, playerName);
        game.initializeNewPlayer(playerName);
    }

    game.printStartSummary();
    game.run();
    return 0;
}
