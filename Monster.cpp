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
int Monster::getObjectifMercy() const { return mercyGoal; }
const vector<string>& Monster::getActionsAct() const { return actIds; }

void Monster::modifierMercy(int delta) {
    mercy += delta;
    mercy = max(0, min(mercy, mercyGoal));
}

bool Monster::peutEtreEpargne() const { return mercy >= mercyGoal; }

int NormalMonster::nombreActionsAct() const { return 2; }
MonsterCategory NormalMonster::getCategorie() const { return MonsterCategory::NORMAL; }
string NormalMonster::getNomCategorie() const { return "NORMAL"; }
unique_ptr<Monster> NormalMonster::cloner() const { return make_unique<NormalMonster>(*this); }

int MiniBossMonster::nombreActionsAct() const { return 3; }
MonsterCategory MiniBossMonster::getCategorie() const { return MonsterCategory::MINIBOSS; }
string MiniBossMonster::getNomCategorie() const { return "MINIBOSS"; }
unique_ptr<Monster> MiniBossMonster::cloner() const { return make_unique<MiniBossMonster>(*this); }

int BossMonster::nombreActionsAct() const { return 4; }
MonsterCategory BossMonster::getCategorie() const { return MonsterCategory::BOSS; }
string BossMonster::getNomCategorie() const { return "BOSS"; }
unique_ptr<Monster> BossMonster::cloner() const { return make_unique<BossMonster>(*this); }
