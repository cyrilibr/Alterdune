#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

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
        const char* term = getenv("TERM");
        return term && string(term) != "dumb";
    }

    static string paint(const string& text, const char* color) {
        if (!colorsEnabled()) return text;
        return string(color) + text + reset;
    }

    static string divider(char fill = '=') { return string(72, fill); }

    static void header(const string& text) {
        cout << "\n" << paint(divider('='), accent) << "\n";
        cout << paint("  " + text, title) << "\n";
        cout << paint(divider('='), accent) << "\n";
    }
    static void printTitleScreen() {
        cout << paint(divider('='), accent) << "\n";
    
        vector<string> logo = {
            "      _    _   _____ _____ ____  ____  _   _ _   _ _____ ",
            "     / \\  | | |_   _| ____|  _ \\|  _ \\| | | | \\ | | ____|",
            "    / _ \\ | |   | | |  _| | |_) | | | | | | |  \\| |  _|  ",
            "   / ___ \\| |___| | | |___|  _ <| |_| | |_| | |\\  | |___ ",
            "  /_/   \\_\\_____|_| |_____|_| \\_\\____/ \\___/|_| \\_|_____|"
        };
    
        for (const string& line : logo) {
            cout << paint(line, title) << "\n";
        }
    
        cout << paint("                         ALTERDUNE", soft) << "\n";
        cout << paint("                Mini-RPG Console Oriente Objet", soft) << "\n";
        cout << paint(divider('='), accent) << "\n";
    }

    static string gauge(int current, int total, int width = 24) {
        if (total <= 0) total = 1;
        current = max(0, min(current, total));

        int filled = static_cast<int>((static_cast<double>(current) / static_cast<double>(total)) * width);
        string bar = "[";
        for (int i = 0; i < width; ++i) {
            bar += (i < filled ? '#' : '.');
        }
        bar += "]";
        return bar;
    }

    static void printBattleMenu() {
        cout << paint(divider('-'), muted) << "\n";
        cout << paint(" [1] FIGHT ", danger)
                  << paint("[2] ACT ", soft)
                  << paint("[3] ITEM ", success)
                  << paint("[4] MERCY", warn) << "\n";
        cout << paint(divider('-'), muted) << "\n";
    }

    static void printMenuBlock(const string& titleText, const vector<string>& options) {
        header(titleText);
        for (const string& option : options) {
            cout << "  " << option << "\n";
        }
        cout << paint(divider('-'), muted) << "\n";
    }
};
