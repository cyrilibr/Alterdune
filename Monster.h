#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Character.h"
using namespace std;

enum class MonsterCategory { NORMAL, MINIBOSS, BOSS };

enum class CombatAction { FIGHT = 1, ACT = 2, ITEM = 3, MERCY = 4 };

class Monster : public Character {
private:
    int mercy;
    int mercyGoal;
    vector<string> actIds;

public:
    Monster(string name,
            int maxHp,
            int attack,
            int defense,
            int mercyGoal,
            vector<string> actIds);

    int getMercy() const;
    int getObjectifMercy() const;
    const vector<string>& getActionsAct() const;

    void modifierMercy(int delta);
    bool peutEtreEpargne() const;

    virtual int nombreActionsAct() const = 0;
    virtual MonsterCategory getCategorie() const = 0;
    virtual string getNomCategorie() const = 0;
    virtual unique_ptr<Monster> cloner() const = 0;
};

class NormalMonster : public Monster {
public:
    using Monster::Monster;
    int nombreActionsAct() const override;
    MonsterCategory getCategorie() const override;
    string getNomCategorie() const override;
    unique_ptr<Monster> cloner() const override;
};

class MiniBossMonster : public Monster {
public:
    using Monster::Monster;
    int nombreActionsAct() const override;
    MonsterCategory getCategorie() const override;
    string getNomCategorie() const override;
    unique_ptr<Monster> cloner() const override;
};

class BossMonster : public Monster {
public:
    using Monster::Monster;
    int nombreActionsAct() const override;
    MonsterCategory getCategorie() const override;
    string getNomCategorie() const override;
    unique_ptr<Monster> cloner() const override;
};
