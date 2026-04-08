# ALTERDUNE - Mini RPG Console C++

ALTERDUNE est un mini-RPG console orienté objet (C++) avec combats type **FIGHT / ACT / ITEM / MERCY**.

## Compilation

Depuis la racine du projet :

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Game.cpp Character.cpp Player.cpp Monster.cpp -o alterdune
```

## Lancer le jeu

```bash
./alterdune
```

## Sauvegarder / Charger

- Au lancement : choisir **"Charger la sauvegarde"** pour reprendre `savegame.txt`.
- Dans le menu principal :
  - **6) Sauvegarder la partie**
  - **7) Charger une sauvegarde**
- Si aucune sauvegarde n'existe, le jeu continue normalement en nouvelle partie.

## Journal de partie

Le menu principal contient désormais **"Journal de partie"**.
Le journal enregistre les événements majeurs (combats, actions, dégâts, items, issues, fin) et est sauvegardé dans `savegame.txt`.

## Exemple de fichier de sauvegarde

Un exemple est fourni dans `savegame_example.txt`.
