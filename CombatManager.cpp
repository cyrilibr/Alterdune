#include "CombatManager.h"

#include <iostream>
#include <limits>
#include <string>
using namespace std;

namespace {
int lireChoix(int min, int max) {
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

int randomDamage(mt19937& rng, int defenderMaxHp) {
    uniform_int_distribution<int> distribution(0, defenderMaxHp);
    return distribution(rng);
}
}

ResultatCombat CombatManager::runCombat(Player& joueur,
                                      Monster& monstre,
                                      const map<string, ActDefinition>& catalogueAct,
                                      mt19937& rng) {
    ResultatCombat resultat;
    resultat.entree = {monstre.getNom(), monstre.getCategorie(), monstre.getHpMax(), monstre.getAttaque(), monstre.getDefense(), false};

    while (joueur.estVivant() && monstre.estVivant()) {
        cout << "\n--- TOUR DU JOUEUR ---\n";
        cout << joueur.getNom() << " HP: " << joueur.getHp() << "/" << joueur.getHpMax() << "\n";
        cout << monstre.getNom() << " HP: " << monstre.getHp() << "/" << monstre.getHpMax()
                  << " | Mercy: " << monstre.getMercy() << "/" << monstre.getObjectifMercy() << "\n";
        cout << "1) FIGHT\n2) ACT\n3) ITEM\n4) MERCY\n";
        cout << "Choix: ";

        const int choix = lireChoix(1, 4);

        if (choix == 1) {
            const int degats = randomDamage(rng, monstre.getHpMax());
            if (degats == 0) {
                cout << "Votre attaque rate !\n";
            } else {
                monstre.recevoirDegats(degats);
                joueur.ajouterDegatsInfliges(degats);
                cout << "Vous infligez " << degats << " degats a " << monstre.getNom() << ".\n";
            }

            if (!monstre.estVivant()) {
                resultat.joueurAGagne = true;
                resultat.monstreEpargne = false;
                resultat.entree.epargne = false;
                break;
            }
        } else if (choix == 2) {
            const auto& ids = monstre.getActionsAct();
            cout << "Actions ACT disponibles:\n";
            for (size_t i = 0; i < ids.size(); ++i) {
                cout << i + 1 << ") " << ids[i] << "\n";
            }

            cout << "Choisissez une action: ";
            const int actChoice = lireChoix(1, static_cast<int>(ids.size()));
            const string& idAction = ids[static_cast<size_t>(actChoice - 1)];
            const ActDefinition& action = catalogueAct.at(idAction);

            const int mercyBefore = monstre.getMercy();
            monstre.modifierMercy(action.mercyImpact);
            const int mercyAfter = monstre.getMercy();

            cout << action.text << "\n";
            cout << "Mercy: " << mercyBefore << " -> " << mercyAfter << "\n";
        } else if (choix == 3) {
            const auto& inventaire = joueur.getInventaire();
            if (inventaire.empty()) {
                cout << "Aucun item disponible.\n";
            } else if (!joueur.aUnItemUtilisable()) {
                cout << "Tous les items sont a quantite 0.\n";
            } else {
                cout << "Items:\n";
                for (size_t i = 0; i < inventaire.size(); ++i) {
                    const Item& item = inventaire[i];
                    cout << i + 1 << ") " << item.name << " x" << item.quantity
                         << " (soin " << item.value << ")\n";
                }

                cout << "0) Retour\n";
                cout << "Choisissez un item: ";
                const int itemChoice = lireChoix(0, static_cast<int>(inventaire.size()));

                if (itemChoice == 0) {
                    continue;
                }

                int soinEffectue = 0;
                string nomItem;

                if (!joueur.utiliserItem(static_cast<size_t>(itemChoice - 1), soinEffectue, nomItem)) {
                    cout << "Item inutilisable (quantite 0 ou index invalide).\n";
                } else {
                    cout << nomItem << " utilise. HP recuperes: " << soinEffectue << "\n";
                    cout << "HP actuel: " << joueur.getHp() << "/" << joueur.getHpMax() << "\n";
                }
            }
        } else {
            if (monstre.peutEtreEpargne()) {
                resultat.joueurAGagne = true;
                resultat.monstreEpargne = true;
                resultat.entree.epargne = true;
                break;
            }

            cout << "Mercy insuffisante: " << monstre.getMercy() << "/" << monstre.getObjectifMercy() << "\n";
        }

        if (!monstre.estVivant() || resultat.monstreEpargne) {
            break;
        }

        cout << "\n--- TOUR DU MONSTRE ---\n";
        const int degatsJoueur = randomDamage(rng, joueur.getHpMax());

        if (degatsJoueur == 0) {
            cout << monstre.getNom() << " rate son attaque.\n";
        } else {
            joueur.recevoirDegats(degatsJoueur);
            joueur.ajouterDegatsRecus(degatsJoueur);
            cout << monstre.getNom() << " inflige " << degatsJoueur << " degats.\n";
        }

        if (!joueur.estVivant()) {
            resultat.joueurEstMort = true;
            break;
        }
    }

    return resultat;
}
