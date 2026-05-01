#include <iostream>
#include <string>

#include "Game.h"
#include "UiTheme.h"
using namespace std;

int main() {
    UiTheme::printTitleScreen();

    Game game("Aventurier");
    if (!game.chargerDonnees("items.csv", "monsters.csv")) {
        cerr << "Le jeu ne peut pas demarrer sans ses fichiers de donnees.\n";
        return 1;
    }

    cout << "Entrez le nom de votre personnage: ";
    string nomJoueur;
    getline(cin, nomJoueur);

    game.initialiserNouveauJoueur(nomJoueur);
    game.afficherResumeDepart();
    game.run();

    return 0;
}
