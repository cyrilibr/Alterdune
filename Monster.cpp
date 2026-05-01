#include "Monster.h"

#include <algorithm>
#include <utility>
using namespace std;

Monster::Monster(string name,
                 int maxHp,
                 int attack,
                 int defense,
                 int mercyGoal,
                 vector<string> actIds)
    : Character(move(name), maxHp, attack, defense), mercy(0), mercyGoal(mercyGoal), actIds(move(actIds)) {}

int Monster::getMercy() const { return mercy; }
int Monster::getMercyGoal() const { return mercyGoal; }
const vector<string>& Monster::getActIds() const { return actIds; }

void Monster::adjustMercy(int delta) {
    mercy += delta;
    mercy = max(0, min(mercy, mercyGoal));
}

bool Monster::canSpare() const { return mercy >= mercyGoal; }

int NormalMonster::actCount() const { return 2; }
MonsterCategory NormalMonster::category() const { return MonsterCategory::NORMAL; }
string NormalMonster::categoryName() const { return "NORMAL"; }
unique_ptr<Monster> NormalMonster::clone() const { return make_unique<NormalMonster>(*this); }

int MiniBossMonster::actCount() const { return 3; }
MonsterCategory MiniBossMonster::category() const { return MonsterCategory::MINIBOSS; }
string MiniBossMonster::categoryName() const { return "MINIBOSS"; }
unique_ptr<Monster> MiniBossMonster::clone() const { return make_unique<MiniBossMonster>(*this); }

int BossMonster::actCount() const { return 4; }
MonsterCategory BossMonster::category() const { return MonsterCategory::BOSS; }
string BossMonster::categoryName() const { return "BOSS"; }
unique_ptr<Monster> BossMonster::clone() const { return make_unique<BossMonster>(*this); }
