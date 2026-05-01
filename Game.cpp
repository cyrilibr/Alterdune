#include "Game.h"

#include <iostream>
#include <limits>

#include "ActCatalog.h"
#include "CombatManager.h"
#include "CsvLoader.h"
using namespace std;

Game::Game(const string& playerName)
    : joueur(playerName, 120, 12, 8),
      catalogueAct(ActCatalog::build()),
      rng(random_device{}()) {}

bool Game::chargerDonnees(const string& itemsPath, const string& monstersPath) {
    vector<Item> items;
    if (!CsvLoader::loadItems(itemsPath, items)) {
        return false;
    }

    vector<unique_ptr<Monster>> monstres;
    if (!CsvLoader::loadMonsters(monstersPath, catalogueAct, monstres)) {
        return false;
    }

    joueur.definirInventaire(move(items));
    monstresDisponibles = move(monstres);
    return true;
}

void Game::initialiserNouveauJoueur(const string& playerName) {
    if (playerName.empty()) {
        joueur.definirNom("Aventurier");
    } else {
        joueur.definirNom(playerName);
    }

    joueur.definirHpMax(120);
    joueur.definirHpActuels(120);
    joueur.definirProgression(0, 0, 0);
    joueur.definirStatistiquesAvancees(0, 0, 0, 0);
    bestiaire.clear();
}

void Game::afficherResumeDepart() const {
    cout << "\n===== RESUME DE LA PARTIE =====\n";
    cout << "Nom du joueur: " << joueur.getNom() << "\n";
    cout << "HP: " << joueur.getHp() << "/" << joueur.getHpMax() << "\n";
    cout << "Inventaire initial:\n";

    const auto& inventaire = joueur.getInventaire();
    if (inventaire.empty()) {
        cout << "- (aucun item)\n";
    } else {
        for (const Item& item : inventaire) {
            cout << "- " << item.name << " x" << item.quantity << "\n";
        }
    }

    cout << "===============================\n\n";
}

int Game::lireChoix(int min, int max) {
    while (true) {
        int choix = 0;
        cin >> choix;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entree invalide. Reessayez: ";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choix < min || choix > max) {
            cout << "Choix hors limites. Reessayez: ";
            continue;
        }

        return choix;
    }
}

void Game::run() {
    bool quitter = false;

    while (!quitter) {
        if (joueur.getVictoires() >= 10) {
            afficherFin();
            break;
        }

        cout << "\n===== MENU PRINCIPAL =====\n";
        cout << "1. Bestiaire\n";
        cout << "2. Demarrer un combat\n";
        cout << "3. Statistiques du personnage\n";
        cout << "4. Items\n";
        cout << "5. Quitter\n";
        cout << "Votre choix: ";

        const int choix = lireChoix(1, 5);

        if (choix == 1) {
            afficherBestiaire();
        } else if (choix == 2) {
            demarrerCombat();
        } else if (choix == 3) {
            joueur.afficherStatistiques();
        } else if (choix == 4) {
            afficherMenuInventaire(true);
        } else {
            quitter = true;
        }

        if (!joueur.estVivant()) {
            cout << "Vous etes tombe au combat. Partie perdue.\n";
            quitter = true;
        }
    }
}

void Game::afficherBestiaire() const {
    cout << "\n===== BESTIAIRE =====\n";

    if (bestiaire.empty()) {
        cout << "Aucun monstre vaincu pour le moment.\n";
        return;
    }

    for (const EntreeBestiaire& entree : bestiaire) {
        cout << "Nom: " << entree.nom << "\n";
        cout << "Categorie: " << categorieEnTexte(entree.categorie) << "\n";
        cout << "HP max: " << entree.hpMax << " | ATK: " << entree.attaque << " | DEF: " << entree.defense << "\n";
        cout << "Resultat: " << (entree.epargne ? "Epargne" : "Tue") << "\n";
        cout << "-----------------------------\n";
    }
}

void Game::afficherMenuInventaire(bool horsCombat) {
    if (!horsCombat) {
        return;
    }

    while (true) {
        const auto& inventaire = joueur.getInventaire();

        cout << "\n===== ITEMS =====\n";
        if (inventaire.empty()) {
            cout << "Inventaire vide.\n";
            return;
        }

        for (size_t i = 0; i < inventaire.size(); ++i) {
            const Item& item = inventaire[i];
            cout << i + 1 << ". " << item.name << " x" << item.quantity
                 << " (soigne " << item.value << " HP)\n";
        }

        cout << "0. Retour\n";
        cout << "Choisissez un item: ";

        const int choix = lireChoix(0, static_cast<int>(inventaire.size()));
        if (choix == 0) {
            return;
        }

        int soinEffectue = 0;
        string nomItem;

        if (!joueur.utiliserItem(static_cast<size_t>(choix - 1), soinEffectue, nomItem)) {
            cout << "Cet item ne peut pas etre utilise (quantite a 0).\n";
            continue;
        }

        cout << nomItem << " utilise. HP recuperes: " << soinEffectue << "\n";
        cout << "HP actuel: " << joueur.getHp() << "/" << joueur.getHpMax() << "\n";
    }
}

Monster& Game::choisirMonstreAleatoire() {
    uniform_int_distribution<size_t> distribution(0, monstresDisponibles.size() - 1);
    return *monstresDisponibles[distribution(rng)];
}

void Game::demarrerCombat() {
    joueur.ajouterCombat();

    Monster& modeleMonstre = choisirMonstreAleatoire();
    unique_ptr<Monster> monstre = modeleMonstre.cloner();

    cout << "\n===== DEBUT DU COMBAT =====\n";
    cout << "Un " << monstre->getNomCategorie() << " apparait: " << monstre->getNom() << "\n";

    ResultatCombat resultat = CombatManager::runCombat(joueur, *monstre, catalogueAct, rng);

    if (resultat.joueurAGagne) {
        bestiaire.push_back(resultat.entree);

        if (resultat.monstreEpargne) {
            joueur.ajouterMonstreEpargne();
            cout << "Victoire pacifique ! " << monstre->getNom() << " est epargne.\n";
        } else {
            joueur.ajouterMonstreTue();
            cout << "Victoire ! " << monstre->getNom() << " est vaincu.\n";
        }

        cout << "Victoires: " << joueur.getVictoires() << "/10\n";
    } else if (resultat.joueurEstMort) {
        cout << "Defaite immediate: " << joueur.getNom() << " est tombe a 0 HP.\n";
    }
}

void Game::afficherFin() const {
    cout << "\n===== FIN DE PARTIE =====\n";

    if (joueur.getMonstresTues() > 0 && joueur.getMonstresEpargnes() == 0) {
        cout << "FIN GENOCIDAIRE\n";
    } else if (joueur.getMonstresEpargnes() > 0 && joueur.getMonstresTues() == 0) {
        cout << "FIN PACIFISTE\n";
    } else {
        cout << "FIN NEUTRE\n";
    }

    cout << "Bilan final: tues=" << joueur.getMonstresTues()
         << ", epargnes=" << joueur.getMonstresEpargnes()
         << ", victoires=" << joueur.getVictoires() << "/10\n";
}

string Game::categorieEnTexte(MonsterCategory categorie) {
    if (categorie == MonsterCategory::NORMAL) {
        return "NORMAL";
    }
    if (categorie == MonsterCategory::MINIBOSS) {
        return "MINIBOSS";
    }
    return "BOSS";
}
