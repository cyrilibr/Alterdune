#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Character.h"

enum class MonsterCategory { NORMAL, MINIBOSS, BOSS };

enum class CombatAction { FIGHT = 1, ACT = 2, ITEM = 3, MERCY = 4 };

class Monster : public Character {
protected:
    int mercy;
    int mercyGoal;
    std::vector<std::string> actIds;

public:
    Monster(std::string name,
            int maxHp,
            int attack,
            int defense,
            int mercyGoal,
            std::vector<std::string> actIds);

    int getMercy() const;
    int getMercyGoal() const;
    const std::vector<std::string>& getActIds() const;

    void adjustMercy(int delta);

    bool canSpare() const;

    virtual MonsterCategory category() const = 0;
    virtual std::string categoryName() const = 0;
    virtual std::unique_ptr<Monster> clone() const = 0;
};

class NormalMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override;
    MonsterCategory category() const override;
    std::string categoryName() const override;
    std::unique_ptr<Monster> clone() const override;
};

class MiniBossMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override;
    MonsterCategory category() const override;
    std::string categoryName() const override;
    std::unique_ptr<Monster> clone() const override;
};

class BossMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override;
    MonsterCategory category() const override;
    std::string categoryName() const override;
    std::unique_ptr<Monster> clone() const override;
};
