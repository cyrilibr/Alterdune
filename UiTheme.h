#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

struct UiTheme {
    static constexpr const char* reset = "\033[0m";
    static constexpr const char* accent = "\033[38;5;45m";
    static constexpr const char* soft = "\033[38;5;111m";
    static constexpr const char* warn = "\033[38;5;214m";
    static constexpr const char* danger = "\033[38;5;203m";
    static constexpr const char* success = "\033[38;5;120m";
    static constexpr const char* title = "\033[1;38;5;51m";

    static bool colorsEnabled() {
        const char* term = std::getenv("TERM");
        return term && std::string(term) != "dumb";
    }

    static std::string paint(const std::string& text, const char* color) {
        if (!colorsEnabled()) return text;
        return std::string(color) + text + reset;
    }

    static std::string divider(char fill = '=') { return std::string(64, fill); }

    static void header(const std::string& text) {
        std::cout << "\n" << paint(divider('='), accent) << "\n";
        std::cout << paint("  " + text, title) << "\n";
        std::cout << paint(divider('='), accent) << "\n";
    }

    static std::string gauge(int current, int total, int width = 22) {
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
};
