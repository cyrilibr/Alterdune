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
    int getMercyGoal() const;
    const vector<string>& getActIds() const;

    void adjustMercy(int delta);
    bool canSpare() const;

    virtual int actCount() const = 0;
    virtual MonsterCategory category() const = 0;
    virtual string categoryName() const = 0;
    virtual unique_ptr<Monster> clone() const = 0;
};

class NormalMonster : public Monster {
public:
    using Monster::Monster;
    int actCount() const override;
    MonsterCategory category() const override;
    string categoryName() const override;
    unique_ptr<Monster> clone() const override;
};

class MiniBossMonster : public Monster {
public:
    using Monster::Monster;
    int actCount() const override;
    MonsterCategory category() const override;
    string categoryName() const override;
    unique_ptr<Monster> clone() const override;
};

class BossMonster : public Monster {
public:
    using Monster::Monster;
    int actCount() const override;
    MonsterCategory category() const override;
    string categoryName() const override;
    unique_ptr<Monster> clone() const override;
};
