#include "Game.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "UiTheme.h"

std::string trim(const std::string& text) {
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

Game::Game(const std::string& playerName)
    : player(playerName, 120, 12, 8), rng(std::random_device{}()), savePath("savegame.txt") {
    buildActCatalog();
}

bool Game::loadData(const std::string& itemsPath, const std::string& monstersPath) {
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

void Game::initializeNewPlayer(const std::string& playerName) {
    std::string safeName = trim(playerName);
    if (safeName.empty()) safeName = "Aventurier";

    player.setName(safeName);
    player.setMaxHpAndClamp(120);
    player.setCurrentHpClamped(120);
    player.setProgress(0, 0, 0);
    player.setAdvancedStats(0, 0, 0, 0);
    beastiary.clear();
    journal.clear();
    addJournal("Nouvelle partie creee pour " + safeName + ".");
}

void Game::printStartSummary() const {
    UiTheme::header("RESUME DE LA PARTIE");
    std::cout << " Joueur   : " << UiTheme::paint(player.getName(), UiTheme::soft) << "\n";
    std::cout << " HP       : " << player.getHp() << "/" << player.getMaxHp() << " "
              << UiTheme::gauge(player.getHp(), player.getMaxHp()) << "\n";
    std::cout << " Inventaire initial:\n";
    const auto& inv = player.getInventory();
    if (inv.empty()) {
        std::cout << "  - " << UiTheme::paint("(aucun item)", UiTheme::warn) << "\n";
    }
    for (const auto& item : inv) {
        std::cout << "  - " << UiTheme::paint(item.name, UiTheme::soft) << " x" << item.quantity
                  << " (" << itemTypeToString(item.type) << ", valeur " << item.value << ")\n";
    }
    std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}

void Game::run() {
    bool quit = false;
    while (!quit) {
        if (player.getVictories() >= 10) {
            printEnding();
            break;
        }

        UiTheme::printMenuBlock("MENU PRINCIPAL ALTERDUNE",
                                {
                                    "1) Bestiaire",
                                    "2) Demarrer un combat",
                                    "3) Statistiques du personnage",
                                    "4) Items",
                                    "5) Journal de partie",
                                    "6) Sauvegarder la partie",
                                    "7) Charger une sauvegarde",
                                    "8) Quitter",
                                });
        std::cout << UiTheme::paint(" Votre choix > ", UiTheme::accent);

        int choice = readChoice(1, 8);

        switch (choice) {
            case 1:
                showBeastiary();
                break;
            case 2:
                startCombat();
                if (!player.isAlive()) {
                    std::cout << "\n" << UiTheme::paint("Vous etes tombe au combat... Partie perdue.", UiTheme::danger) << "\n";
                    addJournal("Defaite du joueur: points de vie tombes a 0.");
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
                showJournal();
                break;
            case 6:
                if (saveGame()) {
                    std::cout << UiTheme::paint("Sauvegarde reussie.", UiTheme::success) << "\n";
                } else {
                    std::cout << UiTheme::paint("Echec de la sauvegarde.", UiTheme::danger) << "\n";
                }
                break;
            case 7:
                if (loadGame()) {
                    std::cout << UiTheme::paint("Sauvegarde chargee avec succes.", UiTheme::success) << "\n";
                } else {
                    std::cout << UiTheme::paint("Impossible de charger la sauvegarde.", UiTheme::warn) << "\n";
                }
                break;
            case 8:
                quit = true;
                break;
            default:
                break;
        }
    }

    std::cout << "\n" << UiTheme::paint("Merci d'avoir joue a ALTERDUNE.", UiTheme::title) << "\n";
}

void Game::buildActCatalog() {
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
        {"THREATEN", "Vous menacez le monstre. Il se fache immediatement.", -25}};

    for (const auto& action : actions) {
        actCatalog[action.id] = action;
    }
}

bool Game::loadItems(const std::string& path, std::vector<Item>& outItems) const {
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

bool Game::loadMonsters(const std::string& path, std::vector<std::unique_ptr<Monster>>& outMonsters) {
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

int Game::readChoice(int min, int max) {
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

int Game::randomDamage(int defenderMaxHp) {
    std::uniform_int_distribution<int> dist(0, defenderMaxHp);
    return dist(rng);
}

void Game::showBeastiary() const {
    UiTheme::header("BESTIAIRE");
    if (beastiary.empty()) {
        std::cout << UiTheme::paint("Aucun monstre vaincu pour le moment.", UiTheme::warn) << "\n";
    } else {
        for (std::size_t i = 0; i < beastiary.size(); ++i) {
            const auto& entry = beastiary[i];
            std::cout << i + 1 << ". " << UiTheme::paint(entry.name, UiTheme::soft)
                      << " [" << categoryToString(entry.category) << "]\n";
            std::cout << "   HP max: " << entry.maxHp << " | ATK: " << entry.attack << " | DEF: " << entry.defense << "\n";
            std::cout << "   Resultat: " << UiTheme::paint(entry.spared ? "Epargne" : "Tue",
                                                           entry.spared ? UiTheme::success : UiTheme::danger)
                      << "\n";
        }
    }
    std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}

std::string Game::categoryToString(MonsterCategory category) {
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

bool Game::parseCategory(const std::string& text, MonsterCategory& outCategory) {
    const std::string up = toUpper(text);
    if (up == "NORMAL") {
        outCategory = MonsterCategory::NORMAL;
        return true;
    }
    if (up == "MINIBOSS") {
        outCategory = MonsterCategory::MINIBOSS;
        return true;
    }
    if (up == "BOSS") {
        outCategory = MonsterCategory::BOSS;
        return true;
    }
    return false;
}

std::string Game::itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::HEAL:
            return "HEAL";
        default:
            return "UNKNOWN";
    }
}

bool Game::parseItemType(const std::string& text, ItemType& outType) {
    if (toUpper(text) == "HEAL") {
        outType = ItemType::HEAL;
        return true;
    }
    return false;
}

void Game::showInventoryMenu(bool outsideCombat) {
    while (true) {
        UiTheme::header("INVENTAIRE");
        const auto& inv = player.getInventory();
        if (inv.empty()) {
            std::cout << "Aucun item disponible.\n";
            std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
            return;
        }

        bool hasItem = false;
        for (std::size_t i = 0; i < inv.size(); ++i) {
            const auto& item = inv[i];
            std::cout << i + 1 << ". " << UiTheme::paint(item.name, UiTheme::soft) << " x" << item.quantity
                      << " - " << itemTypeToString(item.type) << " " << item.value << "\n";
            if (item.quantity > 0) hasItem = true;
        }
        std::cout << "0. Retour\n";
        std::cout << UiTheme::paint("Choisissez un item a utiliser: ", UiTheme::accent);

        int choice = readChoice(0, static_cast<int>(inv.size()));
        if (choice == 0) {
            return;
        }

        std::size_t index = static_cast<std::size_t>(choice - 1);
        auto& editableInv = player.getInventory();
        auto& selected = editableInv[index];

        if (selected.quantity <= 0) {
            std::cout << UiTheme::paint("Cet item est en rupture.", UiTheme::warn) << "\n";
            if (!outsideCombat) return;
            continue;
        }

        int hpBefore = player.getHp();
        player.heal(selected.value);
        int healed = player.getHp() - hpBefore;
        --selected.quantity;
        player.addHealingDone(healed);

        std::cout << UiTheme::paint("+" + std::to_string(healed) + " HP", UiTheme::success)
                  << " avec " << selected.name << ".\n";
        std::cout << "HP actuel: " << player.getHp() << "/" << player.getMaxHp() << "\n";
        addJournal("Item utilise: " + selected.name + " (soin " + std::to_string(healed) + " HP).");

        if (!outsideCombat || !hasItem) {
            return;
        }
    }
}

Monster& Game::randomMonster() {
    std::uniform_int_distribution<std::size_t> dist(0, monsterPool.size() - 1);
    return *monsterPool[dist(rng)];
}

void Game::startCombat() {
    Monster& templateMonster = randomMonster();
    std::unique_ptr<Monster> monster = templateMonster.clone();
    player.addCombat();

    UiTheme::header("DEBUT DU COMBAT");
    std::cout << "Un " << UiTheme::paint(monster->categoryName(), UiTheme::warn)
              << " apparait: " << UiTheme::paint(monster->getName(), UiTheme::soft) << " !\n";
    addJournal("Debut de combat contre " + monster->getName() + " [" + monster->categoryName() + "].");

    CombatResult result = runCombat(*monster);

    if (result.playerWon) {
        beastiary.push_back(result.entry);
        if (result.monsterSpared) {
            player.addSpared();
            std::cout << UiTheme::paint("[MERCY] Victoire pacifique ! ", UiTheme::success) << monster->getName()
                      << " est epargne.\n";
            addJournal("Monstre epargne: " + monster->getName() + ".");
        } else {
            player.addKill();
            std::cout << UiTheme::paint("[KO] Victoire ! ", UiTheme::success) << monster->getName() << " est vaincu.\n";
            addJournal("Monstre vaincu: " + monster->getName() + ".");
        }
        std::cout << "Victoires: " << player.getVictories() << "/10\n";
        addJournal("Victoire obtenue. Total victoires: " + std::to_string(player.getVictories()) + ".");
    }
}

CombatResult Game::runCombat(Monster& monster) {
    CombatResult result;

    result.entry.name = monster.getName();
    result.entry.category = monster.category();
    result.entry.maxHp = monster.getMaxHp();
    result.entry.attack = monster.getAttack();
    result.entry.defense = monster.getDefense();

    while (player.isAlive() && monster.isAlive()) {
        UiTheme::header("TOUR DU JOUEUR");
        std::cout << player.getName() << " HP: " << player.getHp() << "/" << player.getMaxHp()
                  << " " << UiTheme::gauge(player.getHp(), player.getMaxHp()) << "\n";
        std::cout << monster.getName() << " HP: " << monster.getHp() << "/" << monster.getMaxHp()
                  << " " << UiTheme::gauge(monster.getHp(), monster.getMaxHp())
                  << " | Mercy: " << monster.getMercy() << "/" << monster.getMercyGoal() << "\n";
        UiTheme::printBattleMenu();
        std::cout << UiTheme::paint("Choix: ", UiTheme::accent);

        int choice = readChoice(1, 4);
        CombatAction action = static_cast<CombatAction>(choice);

        if (action == CombatAction::FIGHT) {
            int damage = randomDamage(monster.getMaxHp());
            if (damage == 0) {
                std::cout << UiTheme::paint("Votre attaque rate completement !", UiTheme::warn) << "\n";
                addJournal("Action FIGHT: ratee.");
            } else {
                monster.takeDamage(damage);
                player.addDamageDealt(damage);
                std::cout << UiTheme::paint("Vous infligez " + std::to_string(damage) + " degats.", UiTheme::danger)
                          << " Cible: " << monster.getName() << ".\n";
                addJournal("Action FIGHT: " + std::to_string(damage) + " degats infliges a " + monster.getName() + ".");
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
            addJournal("Action ITEM choisie.");
            showInventoryMenu(false);
        } else if (action == CombatAction::MERCY) {
            addJournal("Action MERCY tentee.");
            if (monster.canSpare()) {
                result.playerWon = true;
                result.monsterSpared = true;
                result.entry.spared = true;
                break;
            }
            std::cout << UiTheme::paint("Mercy insuffisante ! ", UiTheme::warn)
                      << "(" << monster.getMercy() << "/" << monster.getMercyGoal() << ")\n";
        }

        if (!monster.isAlive()) {
            result.playerWon = true;
            result.monsterSpared = false;
            result.entry.spared = false;
            break;
        }

        UiTheme::header("TOUR DU MONSTRE");
        int damageToPlayer = randomDamage(player.getMaxHp());
        if (damageToPlayer == 0) {
            std::cout << monster.getName() << " vous manque !\n";
            addJournal("Le monstre " + monster.getName() + " rate son attaque.");
        } else {
            player.takeDamage(damageToPlayer);
            player.addDamageTaken(damageToPlayer);
            std::cout << UiTheme::paint(monster.getName() + " inflige " + std::to_string(damageToPlayer) + " degats.", UiTheme::danger)
                      << "\n";
            addJournal("Degats recus: " + std::to_string(damageToPlayer) + " (" + monster.getName() + ").");
        }

        if (!player.isAlive()) {
            result.playerDied = true;
            break;
        }
    }

    return result;
}

void Game::performAct(Monster& monster) {
    const auto& ids = monster.getActIds();

    UiTheme::header("ACTIONS ACT");
    for (std::size_t i = 0; i < ids.size(); ++i) {
        std::cout << i + 1 << ". " << ids[i] << "\n";
    }
    std::cout << UiTheme::paint("Choisissez une action: ", UiTheme::accent);

    int choice = readChoice(1, static_cast<int>(ids.size()));
    const std::string& chosenId = ids[static_cast<std::size_t>(choice - 1)];
    const ActDefinition& action = actCatalog.at(chosenId);

    std::cout << UiTheme::paint(action.text, UiTheme::soft) << "\n";
    int before = monster.getMercy();
    monster.adjustMercy(action.mercyImpact);
    int after = monster.getMercy();

    int delta = after - before;
    std::cout << "Mercy: " << before << " -> " << after;
    if (delta > 0) {
        std::cout << " (+" << delta << ")\n";
    } else {
        std::cout << " (" << delta << ")\n";
    }

    addJournal("Action ACT: " + chosenId + " (mercy " + std::to_string(before) + "->" + std::to_string(after) + ").");
}

void Game::printEnding() {
    UiTheme::header("FIN DE PARTIE");
    std::cout << UiTheme::paint("Vous avez atteint 10 victoires !", UiTheme::success) << "\n";
    if (player.getKills() > 0 && player.getSpared() == 0) {
        std::cout << UiTheme::paint("ROUTE GENOCIDAIRE", UiTheme::danger) << "\n";
        std::cout << "Le silence remplace chaque cri. ALTERDUNE se souvient de votre violence.\n";
        addJournal("Fin atteinte: Genocidaire.");
    } else if (player.getSpared() > 0 && player.getKills() == 0) {
        std::cout << UiTheme::paint("ROUTE PACIFISTE", UiTheme::success) << "\n";
        std::cout << "Les monstres racontent votre nom comme une promesse de paix.\n";
        addJournal("Fin atteinte: Pacifiste.");
    } else {
        std::cout << UiTheme::paint("ROUTE NEUTRE", UiTheme::warn) << "\n";
        std::cout << "Votre route reste ambigue: survivre, convaincre, parfois detruire.\n";
        addJournal("Fin atteinte: Neutre.");
    }

    std::cout << "Bilan final -> Tues: " << player.getKills() << ", Epargnes: " << player.getSpared()
              << ", Victoires: " << player.getVictories() << "\n";
    std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}

void Game::addJournal(const std::string& entry) {
    journal.push_back(entry);
}

void Game::showJournal() const {
    UiTheme::header("JOURNAL DE PARTIE");
    if (journal.empty()) {
        std::cout << UiTheme::paint("Le journal est vide pour le moment.", UiTheme::warn) << "\n";
    } else {
        for (std::size_t i = 0; i < journal.size(); ++i) {
            std::cout << i + 1 << ". " << journal[i] << "\n";
        }
    }
    std::cout << UiTheme::paint(UiTheme::divider('='), UiTheme::accent) << "\n";
}

bool Game::saveGame() const {
    std::ofstream file(savePath);
    if (!file) {
        std::cerr << "Erreur: impossible d'ecrire le fichier de sauvegarde.\n";
        return false;
    }

    file << "[PLAYER]\n";
    file << "name=" << player.getName() << "\n";
    file << "hp=" << player.getHp() << "\n";
    file << "maxHp=" << player.getMaxHp() << "\n";
    file << "kills=" << player.getKills() << "\n";
    file << "spared=" << player.getSpared() << "\n";
    file << "victories=" << player.getVictories() << "\n";
    file << "totalCombats=" << player.getTotalCombats() << "\n";
    file << "totalDamageDealt=" << player.getTotalDamageDealt() << "\n";
    file << "totalDamageTaken=" << player.getTotalDamageTaken() << "\n";
    file << "totalHealingDone=" << player.getTotalHealingDone() << "\n";

    file << "[INVENTORY]\n";
    for (const auto& item : player.getInventory()) {
        file << item.name << ';' << itemTypeToString(item.type) << ';' << item.value << ';' << item.quantity << "\n";
    }

    file << "[BESTIARY]\n";
    for (const auto& entry : beastiary) {
        file << entry.name << ';' << categoryToString(entry.category) << ';' << entry.maxHp << ';' << entry.attack << ';'
             << entry.defense << ';' << (entry.spared ? 1 : 0) << "\n";
    }

    file << "[JOURNAL]\n";
    for (const auto& line : journal) {
        file << line << "\n";
    }

    return true;
}

bool Game::loadGame() {
    std::ifstream file(savePath);
    if (!file) {
        std::cerr << "Aucune sauvegarde disponible (" << savePath << ").\n";
        return false;
    }

    std::string line;
    std::string section;

    std::string loadedName = player.getName();
    int hp = player.getHp();
    int maxHp = player.getMaxHp();
    int kills = player.getKills();
    int spared = player.getSpared();
    int victories = player.getVictories();
    int totalCombats = player.getTotalCombats();
    int totalDamageDealt = player.getTotalDamageDealt();
    int totalDamageTaken = player.getTotalDamageTaken();
    int totalHealingDone = player.getTotalHealingDone();

    std::vector<Item> loadedInventory;
    std::vector<BeastiaryEntry> loadedBeastiary;
    std::vector<std::string> loadedJournal;

    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty()) continue;

        if (line.front() == '[' && line.back() == ']') {
            section = line;
            continue;
        }

        if (section == "[PLAYER]") {
            auto eqPos = line.find('=');
            if (eqPos == std::string::npos) {
                std::cerr << "Sauvegarde invalide [PLAYER], ligne " << lineNumber << "\n";
                continue;
            }
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            int parsed = 0;
            if (key == "name") loadedName = value;
            else if (key == "hp" && parseInt(value, parsed)) hp = parsed;
            else if (key == "maxHp" && parseInt(value, parsed)) maxHp = parsed;
            else if (key == "kills" && parseInt(value, parsed)) kills = parsed;
            else if (key == "spared" && parseInt(value, parsed)) spared = parsed;
            else if (key == "victories" && parseInt(value, parsed)) victories = parsed;
            else if (key == "totalCombats" && parseInt(value, parsed)) totalCombats = parsed;
            else if (key == "totalDamageDealt" && parseInt(value, parsed)) totalDamageDealt = parsed;
            else if (key == "totalDamageTaken" && parseInt(value, parsed)) totalDamageTaken = parsed;
            else if (key == "totalHealingDone" && parseInt(value, parsed)) totalHealingDone = parsed;
        } else if (section == "[INVENTORY]") {
            auto cols = split(line, ';');
            if (cols.size() != 4) {
                std::cerr << "Ligne inventaire ignoree (invalide), ligne " << lineNumber << "\n";
                continue;
            }
            ItemType type = ItemType::HEAL;
            int value = 0;
            int qty = 0;
            if (cols[0].empty() || !parseItemType(cols[1], type) || !parseInt(cols[2], value) || !parseInt(cols[3], qty) ||
                value <= 0 || qty < 0) {
                std::cerr << "Ligne inventaire ignoree (valeurs invalides), ligne " << lineNumber << "\n";
                continue;
            }
            loadedInventory.push_back({cols[0], type, value, qty});
        } else if (section == "[BESTIARY]") {
            auto cols = split(line, ';');
            if (cols.size() != 6) {
                std::cerr << "Ligne bestiaire ignoree (invalide), ligne " << lineNumber << "\n";
                continue;
            }

            MonsterCategory category = MonsterCategory::NORMAL;
            int maxHpEntry = 0;
            int atk = 0;
            int def = 0;
            int sparedFlag = 0;
            if (cols[0].empty() || !parseCategory(cols[1], category) || !parseInt(cols[2], maxHpEntry) ||
                !parseInt(cols[3], atk) || !parseInt(cols[4], def) || !parseInt(cols[5], sparedFlag) || maxHpEntry <= 0 ||
                atk < 0 || def < 0) {
                std::cerr << "Ligne bestiaire ignoree (valeurs invalides), ligne " << lineNumber << "\n";
                continue;
            }
            loadedBeastiary.push_back({cols[0], category, maxHpEntry, atk, def, sparedFlag != 0});
        } else if (section == "[JOURNAL]") {
            loadedJournal.push_back(line);
        }
    }

    if (maxHp <= 0) {
        std::cerr << "Sauvegarde invalide: maxHp <= 0\n";
        return false;
    }

    player.setName(loadedName.empty() ? "Aventurier" : loadedName);
    player.setMaxHpAndClamp(maxHp);
    player.setCurrentHpClamped(hp);
    player.setProgress(kills, spared, victories);
    player.setAdvancedStats(totalCombats, totalDamageDealt, totalDamageTaken, totalHealingDone);

    if (!loadedInventory.empty()) {
        player.setInventory(std::move(loadedInventory));
    }

    beastiary = std::move(loadedBeastiary);
    journal = std::move(loadedJournal);
    addJournal("Sauvegarde chargee depuis " + savePath + ".");
    return true;
}

bool Game::hasSaveFile() const {
    std::ifstream file(savePath);
    return static_cast<bool>(file);
}
