#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

enum class MonsterCategory { NORMAL, MINIBOSS, BOSS };
enum class ItemType { HEAL };

enum class CombatAction { FIGHT = 1, ACT = 2, ITEM = 3, MERCY = 4 };

struct ActDefinition {
    std::string id;
    std::string text;
    int mercyImpact;
};

struct Item {
    std::string name;
    ItemType type;
    int value;
    int quantity;
};

struct BeastiaryEntry {
    std::string name;
    MonsterCategory category;
    int maxHp;
    int attack;
    int defense;
    bool spared;
};

class Character {
protected:
    std::string name;
    int hp;
    int maxHp;
    int attack;
    int defense;

public:
    Character(std::string name, int maxHp, int attack, int defense)
        : name(std::move(name)), hp(maxHp), maxHp(maxHp), attack(attack), defense(defense) {}

    virtual ~Character() = default;

    const std::string& getName() const { return name; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }

    bool isAlive() const { return hp > 0; }

    void heal(int amount) {
        if (amount <= 0) return;
        hp = std::min(maxHp, hp + amount);
    }

    void takeDamage(int damage) {
        if (damage <= 0) return;
        hp = std::max(0, hp - damage);
    }

    virtual int actCount() const = 0;
};

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
            std::vector<std::string> actIds)
        : Character(std::move(name), maxHp, attack, defense), mercy(0), mercyGoal(mercyGoal), actIds(std::move(actIds)) {}

    int getMercy() const { return mercy; }
    int getMercyGoal() const { return mercyGoal; }
    const std::vector<std::string>& getActIds() const { return actIds; }

    void adjustMercy(int delta) {
        mercy += delta;
        mercy = std::max(0, std::min(mercy, mercyGoal));
    }

    bool canSpare() const { return mercy >= mercyGoal; }

    virtual MonsterCategory category() const = 0;
    virtual std::string categoryName() const = 0;
    virtual std::unique_ptr<Monster> clone() const = 0;
};

class NormalMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override { return 2; }
    MonsterCategory category() const override { return MonsterCategory::NORMAL; }
    std::string categoryName() const override { return "NORMAL"; }
    std::unique_ptr<Monster> clone() const override { return std::make_unique<NormalMonster>(*this); }
};

class MiniBossMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override { return 3; }
    MonsterCategory category() const override { return MonsterCategory::MINIBOSS; }
    std::string categoryName() const override { return "MINIBOSS"; }
    std::unique_ptr<Monster> clone() const override { return std::make_unique<MiniBossMonster>(*this); }
};

class BossMonster : public Monster {
public:
    using Monster::Monster;

    int actCount() const override { return 4; }
    MonsterCategory category() const override { return MonsterCategory::BOSS; }
    std::string categoryName() const override { return "BOSS"; }
    std::unique_ptr<Monster> clone() const override { return std::make_unique<BossMonster>(*this); }
};

class Player : public Character {
private:
    std::vector<Item> inventory;
    int kills;
    int spared;
    int victories;

public:
    Player(std::string name, int maxHp, int attack, int defense)
        : Character(std::move(name), maxHp, attack, defense), kills(0), spared(0), victories(0) {}

    int actCount() const override { return 0; }

    std::vector<Item>& getInventory() { return inventory; }
    const std::vector<Item>& getInventory() const { return inventory; }

    void setInventory(std::vector<Item> loadedItems) { inventory = std::move(loadedItems); }

    int getKills() const { return kills; }
    int getSpared() const { return spared; }
    int getVictories() const { return victories; }

    void addKill() {
        ++kills;
        ++victories;
    }

    void addSpared() {
        ++spared;
        ++victories;
    }

    void printStats() const {
        std::cout << "\n===== STATISTIQUES DU JOUEUR =====\n";
        std::cout << "Nom: " << name << "\n";
        std::cout << "HP: " << hp << "/" << maxHp << "\n";
        std::cout << "Monstres tues: " << kills << "\n";
        std::cout << "Monstres epargnes: " << spared << "\n";
        std::cout << "Victoires: " << victories << "/10\n";
        std::cout << "==================================\n";
    }
};

struct CombatResult {
    bool playerWon = false;
    bool playerDied = false;
    bool monsterSpared = false;
    BeastiaryEntry entry{};
};

static std::string trim(const std::string& text) {
    std::size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }
    std::size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(start, end - start);
}

static std::vector<std::string> split(const std::string& line, char separator) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, separator)) {
        out.push_back(trim(part));
    }
    return out;
}

static std::string toUpper(std::string v) {
    for (char& ch : v) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return v;
}

static bool parseInt(const std::string& text, int& outValue) {
    try {
        std::size_t pos = 0;
        int value = std::stoi(text, &pos);
        if (pos != text.size()) return false;
        outValue = value;
        return true;
    } catch (...) {
        return false;
    }
}

class Game {
private:
    Player player;
    std::vector<std::unique_ptr<Monster>> monsterPool;
    std::map<std::string, ActDefinition> actCatalog;
    std::vector<BeastiaryEntry> beastiary;
    std::mt19937 rng;

public:
    explicit Game(const std::string& playerName)
        : player(playerName, 120, 12, 8), rng(std::random_device{}()) {
        buildActCatalog();
    }

    bool loadData(const std::string& itemsPath, const std::string& monstersPath) {
        std::vector<Item> loadedItems;
        if (!loadItems(itemsPath, loadedItems)) {
            return false;
        }

        std::vector<std::unique_ptr<Monster>> loadedMonsters;
        if (!loadMonsters(monstersPath, loadedMonsters)) {
            return false;
        }

        player.setInventory(std::move(loadedItems));
        monsterPool = std::move(loadedMonsters);
        return true;
    }

    void printStartSummary() const {
        std::cout << "\n===== RESUME DE LA PARTIE =====\n";
        std::cout << "Joueur: " << player.getName() << "\n";
        std::cout << "HP: " << player.getHp() << "/" << player.getMaxHp() << "\n";
        std::cout << "Inventaire initial:\n";
        const auto& inv = player.getInventory();
        if (inv.empty()) {
            std::cout << "  (aucun item)\n";
        }
        for (const auto& item : inv) {
            std::cout << "  - " << item.name << " x" << item.quantity << " (soigne " << item.value << " HP)\n";
        }
        std::cout << "===============================\n";
    }

    void run() {
        bool quit = false;
        while (!quit) {
            if (player.getVictories() >= 10) {
                printEnding();
                break;
            }

            std::cout << "\n===== MENU PRINCIPAL =====\n";
            std::cout << "1. Bestiaire\n";
            std::cout << "2. Demarrer un combat\n";
            std::cout << "3. Statistiques du personnage\n";
            std::cout << "4. Items\n";
            std::cout << "5. Quitter\n";
            std::cout << "Votre choix: ";

            int choice = readChoice(1, 5);

            switch (choice) {
                case 1:
                    showBeastiary();
                    break;
                case 2:
                    startCombat();
                    if (!player.isAlive()) {
                        std::cout << "\nVous etes tombe au combat... Partie perdue.\n";
                        quit = true;
                    }
                    break;
                case 3:
                    player.printStats();
                    break;
                case 4:
                    showInventoryMenu(true);
                    break;
                case 5:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        std::cout << "\nMerci d'avoir joue a ALTERDUNE.\n";
    }

private:
    void buildActCatalog() {
        const std::vector<ActDefinition> actions = {
            {"JOKE", "Vous racontez une blague absurde sur un clavier qui mange des nuages.", 25},
            {"COMPLIMENT", "Vous complimentez sincerement les pixels du monstre.", 20},
            {"INSULT", "Vous critiquez son style vestimentaire binaire. Mauvaise idee.", -20},
            {"DISCUSS", "Vous lancez un debat philosophique sur les croissants quantiques.", 15},
            {"OBSERVE", "Vous observez calmement ses mouvements. L'atmosphere se detend.", 10},
            {"PET", "Vous tentez une caresse diplomatique. Etonnamment efficace.", 30},
            {"OFFER_SNACK", "Vous offrez un snack. Le monstre grignote en silence.", 18},
            {"REASON", "Vous essayez de raisonner le monstre avec logique et empathie.", 22},
            {"DANCE", "Vous improvisez une danse catastrophique mais touchante.", 12},
            {"THREATEN", "Vous menacez le monstre. Il se fache immediatement.", -25}
        };

        for (const auto& action : actions) {
            actCatalog[action.id] = action;
        }
    }

    bool loadItems(const std::string& path, std::vector<Item>& outItems) const {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Erreur: impossible d'ouvrir le fichier items: " << path << "\n";
            return false;
        }

        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            if (line.empty()) continue;

            auto cols = split(line, ';');
            if (cols.size() != 4) {
                std::cerr << "Ligne items mal formee (ignoree) ligne " << lineNumber << ": " << line << "\n";
                continue;
            }

            const std::string itemName = cols[0];
            const std::string itemType = toUpper(cols[1]);
            int value = 0;
            int quantity = 0;

            if (itemName.empty() || itemType != "HEAL" || !parseInt(cols[2], value) || !parseInt(cols[3], quantity) ||
                value <= 0 || quantity < 0) {
                std::cerr << "Ligne items invalide (ignoree) ligne " << lineNumber << ": " << line << "\n";
                continue;
            }

            outItems.push_back({itemName, ItemType::HEAL, value, quantity});
        }

        return true;
    }

    bool loadMonsters(const std::string& path, std::vector<std::unique_ptr<Monster>>& outMonsters) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Erreur: impossible d'ouvrir le fichier monsters: " << path << "\n";
            return false;
        }

        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            if (line.empty()) continue;

            auto cols = split(line, ';');
            if (cols.size() != 10) {
                std::cerr << "Ligne monsters mal formee (ignoree) ligne " << lineNumber << ": " << line << "\n";
                continue;
            }

            std::string categoryText = toUpper(cols[0]);
            std::string name = cols[1];

            int hp = 0;
            int atk = 0;
            int def = 0;
            int mercyGoal = 0;

            if (!parseInt(cols[2], hp) || !parseInt(cols[3], atk) || !parseInt(cols[4], def) || !parseInt(cols[5], mercyGoal) ||
                hp <= 0 || atk < 0 || def < 0 || mercyGoal <= 0 || name.empty()) {
                std::cerr << "Ligne monsters invalide (ignoree) ligne " << lineNumber << ": " << line << "\n";
                continue;
            }

            std::vector<std::string> acts;
            for (std::size_t i = 6; i < cols.size(); ++i) {
                std::string actId = toUpper(cols[i]);
                if (actId == "-" || actId.empty()) {
                    continue;
                }
                if (actCatalog.find(actId) == actCatalog.end()) {
                    std::cerr << "Action ACT inconnue (ignoree) ligne " << lineNumber << ": " << actId << "\n";
                    continue;
                }
                acts.push_back(actId);
            }

            std::unique_ptr<Monster> monster;
            if (categoryText == "NORMAL") {
                if (acts.size() < 2) {
                    std::cerr << "Monstre NORMAL invalide (moins de 2 ACT), ligne " << lineNumber << "\n";
                    continue;
                }
                acts.resize(2);
                monster = std::make_unique<NormalMonster>(name, hp, atk, def, mercyGoal, acts);
            } else if (categoryText == "MINIBOSS") {
                if (acts.size() < 3) {
                    std::cerr << "Monstre MINIBOSS invalide (moins de 3 ACT), ligne " << lineNumber << "\n";
                    continue;
                }
                acts.resize(3);
                monster = std::make_unique<MiniBossMonster>(name, hp, atk, def, mercyGoal, acts);
            } else if (categoryText == "BOSS") {
                if (acts.size() < 4) {
                    std::cerr << "Monstre BOSS invalide (moins de 4 ACT), ligne " << lineNumber << "\n";
                    continue;
                }
                acts.resize(4);
                monster = std::make_unique<BossMonster>(name, hp, atk, def, mercyGoal, acts);
            } else {
                std::cerr << "Categorie inconnue (ignoree) ligne " << lineNumber << ": " << cols[0] << "\n";
                continue;
            }

            outMonsters.push_back(std::move(monster));
        }

        if (outMonsters.empty()) {
            std::cerr << "Erreur: aucun monstre valide charge.\n";
            return false;
        }

        return true;
    }

    static int readChoice(int min, int max) {
        while (true) {
            int choice;
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Entree invalide. Reessayez: ";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice < min || choice > max) {
                std::cout << "Choix hors limite. Reessayez: ";
                continue;
            }

            return choice;
        }
    }

    int randomDamage(int defenderMaxHp) {
        std::uniform_int_distribution<int> dist(0, defenderMaxHp);
        return dist(rng);
    }

    void showBeastiary() const {
        std::cout << "\n===== BESTIAIRE =====\n";
        if (beastiary.empty()) {
            std::cout << "Aucun monstre vaincu pour le moment.\n";
        } else {
            for (std::size_t i = 0; i < beastiary.size(); ++i) {
                const auto& entry = beastiary[i];
                std::cout << i + 1 << ". " << entry.name << " [" << categoryToString(entry.category) << "]\n";
                std::cout << "   HP max: " << entry.maxHp << " | ATK: " << entry.attack << " | DEF: " << entry.defense << "\n";
                std::cout << "   Resultat: " << (entry.spared ? "Epargne" : "Tue") << "\n";
            }
        }
        std::cout << "=====================\n";
    }

    static std::string categoryToString(MonsterCategory category) {
        switch (category) {
            case MonsterCategory::NORMAL:
                return "NORMAL";
            case MonsterCategory::MINIBOSS:
                return "MINIBOSS";
            case MonsterCategory::BOSS:
                return "BOSS";
            default:
                return "INCONNU";
        }
    }

    void showInventoryMenu(bool outsideCombat) {
        while (true) {
            std::cout << "\n===== INVENTAIRE =====\n";
            const auto& inv = player.getInventory();
            if (inv.empty()) {
                std::cout << "Aucun item disponible.\n";
                std::cout << "======================\n";
                return;
            }

            bool hasItem = false;
            for (std::size_t i = 0; i < inv.size(); ++i) {
                const auto& item = inv[i];
                std::cout << i + 1 << ". " << item.name << " x" << item.quantity
                          << " - soigne " << item.value << " HP\n";
                if (item.quantity > 0) hasItem = true;
            }
            std::cout << "0. Retour\n";
            std::cout << "Choisissez un item a utiliser: ";

            int choice = readChoice(0, static_cast<int>(inv.size()));
            if (choice == 0) {
                return;
            }

            std::size_t index = static_cast<std::size_t>(choice - 1);
            auto& editableInv = player.getInventory();
            auto& selected = editableInv[index];

            if (selected.quantity <= 0) {
                std::cout << "Cet item est en rupture.\n";
                if (!outsideCombat) return;
                continue;
            }

            int hpBefore = player.getHp();
            player.heal(selected.value);
            int healed = player.getHp() - hpBefore;
            --selected.quantity;

            std::cout << "Vous utilisez " << selected.name << " et recuperez " << healed << " HP.\n";
            std::cout << "HP actuel: " << player.getHp() << "/" << player.getMaxHp() << "\n";

            if (!outsideCombat || !hasItem) {
                return;
            }
        }
    }

    Monster& randomMonster() {
        std::uniform_int_distribution<std::size_t> dist(0, monsterPool.size() - 1);
        return *monsterPool[dist(rng)];
    }

    void startCombat() {
        Monster& templateMonster = randomMonster();
        std::unique_ptr<Monster> monster = templateMonster.clone();

        std::cout << "\n===== DEBUT DU COMBAT =====\n";
        std::cout << "Un " << monster->categoryName() << " apparait: " << monster->getName() << " !\n";

        CombatResult result = runCombat(*monster);

        if (result.playerWon) {
            beastiary.push_back(result.entry);
            if (result.monsterSpared) {
                player.addSpared();
                std::cout << "Victoire pacifique ! " << monster->getName() << " est epargne.\n";
            } else {
                player.addKill();
                std::cout << "Victoire ! " << monster->getName() << " est vaincu.\n";
            }
            std::cout << "Victoires: " << player.getVictories() << "/10\n";
        }
    }

    CombatResult runCombat(Monster& monster) {
        CombatResult result;

        result.entry.name = monster.getName();
        result.entry.category = monster.category();
        result.entry.maxHp = monster.getMaxHp();
        result.entry.attack = monster.getAttack();
        result.entry.defense = monster.getDefense();

        while (player.isAlive() && monster.isAlive()) {
            std::cout << "\n--- TOUR DU JOUEUR ---\n";
            std::cout << player.getName() << " HP: " << player.getHp() << "/" << player.getMaxHp() << "\n";
            std::cout << monster.getName() << " HP: " << monster.getHp() << "/" << monster.getMaxHp()
                      << " | Mercy: " << monster.getMercy() << "/" << monster.getMercyGoal() << "\n";
            std::cout << "1. FIGHT\n2. ACT\n3. ITEM\n4. MERCY\n";
            std::cout << "Choix: ";

            int choice = readChoice(1, 4);
            CombatAction action = static_cast<CombatAction>(choice);

            if (action == CombatAction::FIGHT) {
                int damage = randomDamage(monster.getMaxHp());
                if (damage == 0) {
                    std::cout << "Votre attaque rate completement !\n";
                } else {
                    monster.takeDamage(damage);
                    std::cout << "Vous infligez " << damage << " degats a " << monster.getName() << ".\n";
                }

                if (!monster.isAlive()) {
                    result.playerWon = true;
                    result.monsterSpared = false;
                    result.entry.spared = false;
                    break;
                }
            } else if (action == CombatAction::ACT) {
                performAct(monster);
            } else if (action == CombatAction::ITEM) {
                showInventoryMenu(false);
            } else if (action == CombatAction::MERCY) {
                if (monster.canSpare()) {
                    result.playerWon = true;
                    result.monsterSpared = true;
                    result.entry.spared = true;
                    break;
                }
                std::cout << "Mercy insuffisante ! (" << monster.getMercy() << "/" << monster.getMercyGoal() << ")\n";
            }

            if (!monster.isAlive()) {
                result.playerWon = true;
                result.monsterSpared = false;
                result.entry.spared = false;
                break;
            }

            std::cout << "\n--- TOUR DU MONSTRE ---\n";
            int damageToPlayer = randomDamage(player.getMaxHp());
            if (damageToPlayer == 0) {
                std::cout << monster.getName() << " vous manque !\n";
            } else {
                player.takeDamage(damageToPlayer);
                std::cout << monster.getName() << " vous inflige " << damageToPlayer << " degats.\n";
            }

            if (!player.isAlive()) {
                result.playerDied = true;
                break;
            }
        }

        return result;
    }

    void performAct(Monster& monster) {
        const auto& ids = monster.getActIds();

        std::cout << "\nActions ACT disponibles:\n";
        for (std::size_t i = 0; i < ids.size(); ++i) {
            std::cout << i + 1 << ". " << ids[i] << "\n";
        }
        std::cout << "Choisissez une action: ";

        int choice = readChoice(1, static_cast<int>(ids.size()));
        const std::string& chosenId = ids[static_cast<std::size_t>(choice - 1)];
        const ActDefinition& action = actCatalog.at(chosenId);

        std::cout << action.text << "\n";
        int before = monster.getMercy();
        monster.adjustMercy(action.mercyImpact);
        int after = monster.getMercy();

        int delta = after - before;
        std::cout << "Mercy: " << before << " -> " << after;
        if (delta > 0) {
            std::cout << " (" << '+' << delta << ")\n";
        } else {
            std::cout << " (" << delta << ")\n";
        }
    }

    void printEnding() const {
        std::cout << "\n===== FIN DE PARTIE =====\n";
        std::cout << "Vous avez atteint 10 victoires !\n";
        if (player.getKills() > 0 && player.getSpared() == 0) {
            std::cout << "Fin Genocidaire: vous avez elimine tous les monstres.\n";
        } else if (player.getSpared() > 0 && player.getKills() == 0) {
            std::cout << "Fin Pacifiste: vous avez epargne tous les monstres.\n";
        } else {
            std::cout << "Fin Neutre: vous avez choisi un chemin melange.\n";
        }

        std::cout << "Bilan final -> Tues: " << player.getKills()
                  << ", Epargnes: " << player.getSpared()
                  << ", Victoires: " << player.getVictories() << "\n";
        std::cout << "=========================\n";
    }
};

int main() {
    std::cout << "Bienvenue dans ALTERDUNE !\n";
    std::cout << "Entrez le nom de votre personnage: ";

    std::string playerName;
    std::getline(std::cin, playerName);
    playerName = trim(playerName);

    if (playerName.empty()) {
        playerName = "Aventurier";
    }

    Game game(playerName);
    if (!game.loadData("items.csv", "monsters.csv")) {
        std::cerr << "Le jeu ne peut pas demarrer sans ses fichiers de donnees.\n";
        return 1;
    }

    game.printStartSummary();
    game.run();
    return 0;
}
