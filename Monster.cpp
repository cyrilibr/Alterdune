#include "Monster.h"

#include <algorithm>
#include <utility>

Monster::Monster(std::string name,
                 int maxHp,
                 int attack,
                 int defense,
                 int mercyGoal,
                 std::vector<std::string> actIds)
    : Character(std::move(name), maxHp, attack, defense), mercy(0), mercyGoal(mercyGoal), actIds(std::move(actIds)) {}

int Monster::getMercy() const { return mercy; }
int Monster::getMercyGoal() const { return mercyGoal; }
const std::vector<std::string>& Monster::getActIds() const { return actIds; }

void Monster::adjustMercy(int delta) {
    mercy += delta;
    mercy = std::max(0, std::min(mercy, mercyGoal));
}

bool Monster::canSpare() const { return mercy >= mercyGoal; }

int NormalMonster::actCount() const { return 2; }
MonsterCategory NormalMonster::category() const { return MonsterCategory::NORMAL; }
std::string NormalMonster::categoryName() const { return "NORMAL"; }
std::unique_ptr<Monster> NormalMonster::clone() const { return std::make_unique<NormalMonster>(*this); }

int MiniBossMonster::actCount() const { return 3; }
MonsterCategory MiniBossMonster::category() const { return MonsterCategory::MINIBOSS; }
std::string MiniBossMonster::categoryName() const { return "MINIBOSS"; }
std::unique_ptr<Monster> MiniBossMonster::clone() const { return std::make_unique<MiniBossMonster>(*this); }

int BossMonster::actCount() const { return 4; }
MonsterCategory BossMonster::category() const { return MonsterCategory::BOSS; }
std::string BossMonster::categoryName() const { return "BOSS"; }
std::unique_ptr<Monster> BossMonster::clone() const { return std::make_unique<BossMonster>(*this); }
