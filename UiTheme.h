#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

struct UiTheme {
    static constexpr const char* reset = "\033[0m";
    static constexpr const char* accent = "\033[38;5;45m";
    static constexpr const char* soft = "\033[38;5;111m";
    static constexpr const char* warn = "\033[38;5;214m";
    static constexpr const char* danger = "\033[38;5;203m";
    static constexpr const char* success = "\033[38;5;120m";
    static constexpr const char* title = "\033[1;38;5;51m";
    static constexpr const char* muted = "\033[38;5;245m";

    static bool colorsEnabled() {
        const char* term = std::getenv("TERM");
        return term && std::string(term) != "dumb";
    }

    static std::string paint(const std::string& text, const char* color) {
        if (!colorsEnabled()) return text;
        return std::string(color) + text + reset;
    }

    static std::string divider(char fill = '=') { return std::string(72, fill); }

    static void header(const std::string& text) {
        std::cout << "\n" << paint(divider('='), accent) << "\n";
        std::cout << paint("  " + text, title) << "\n";
        std::cout << paint(divider('='), accent) << "\n";
    }

    static void printTitleScreen() {
        std::cout << paint(divider('='), accent) << "\n";
        std::cout << paint("      ___    _   _____ _____ ____  ____  _   _ _   _ _____ ", title) << "\n";
        std::cout << paint("     / _ \\  | | |_   _| ____|  _ \\|  _ \\| | | | \\| | ____|", title) << "\n";
        std::cout << paint("    | | | | | |   | | |  _| | |_) | | | | | | |  \\| |  _|  ", title) << "\n";
        std::cout << paint("    | |_| | | |___| | | |___|  _ <| |_| | |_| | |\\  | |___ ", title) << "\n";
        std::cout << paint("     \\___/  |_____|_| |_____|_| \\_\\____/ \\___/|_| \\_|_____|", title) << "\n";
        std::cout << paint("                Mini-RPG Console Orienté Objet", soft) << "\n";
        std::cout << paint(divider('='), accent) << "\n";
    }

    static std::string gauge(int current, int total, int width = 24) {
        if (total <= 0) total = 1;
        current = std::max(0, std::min(current, total));

        int filled = static_cast<int>((static_cast<double>(current) / static_cast<double>(total)) * width);
        std::string bar = "[";
        for (int i = 0; i < width; ++i) {
            bar += (i < filled ? '#' : '.');
        }
        bar += "]";
        return bar;
    }

    static void printBattleMenu() {
        std::cout << paint(divider('-'), muted) << "\n";
        std::cout << paint(" [1] FIGHT ", danger)
                  << paint("[2] ACT ", soft)
                  << paint("[3] ITEM ", success)
                  << paint("[4] MERCY", warn) << "\n";
        std::cout << paint(divider('-'), muted) << "\n";
    }

    static void printMenuBlock(const std::string& titleText, const std::vector<std::string>& options) {
        header(titleText);
        for (const std::string& option : options) {
            std::cout << "  " << option << "\n";
        }
        std::cout << paint(divider('-'), muted) << "\n";
    }
};
