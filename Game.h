#pragma once

#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "ActDefinition.h"
#include "BestiaryEntry.h"
#include "Monster.h"
#include "Player.h"
using namespace std;

class Game {
public:
    explicit Game(const string& playerName);

    bool chargerDonnees(const string& itemsPath, const string& monstersPath);
    void initialiserNouveauJoueur(const string& playerName);
    void afficherResumeDepart() const;
    void run();

    static int lireChoix(int min, int max);

private:
    Player joueur;
    vector<unique_ptr<Monster>> monstresDisponibles;
    map<string, ActDefinition> catalogueAct;
    vector<EntreeBestiaire> bestiaire;
    mt19937 rng;

    void afficherBestiaire() const;
    void afficherMenuInventaire(bool outsideCombat);
    Monster& choisirMonstreAleatoire();
    void demarrerCombat();
    void afficherFin() const;
    static string categorieEnTexte(MonsterCategory categorie);
};
