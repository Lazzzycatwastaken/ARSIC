#include "ascii_art.h"
#include <iostream>
#include <string>
#include <algorithm>

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " IMAGE STYLE COLORS [WIDTH]\n";
        std::cerr << "  STYLE: clean | high_fidelity | block\n";
        std::cerr << "  COLORS: yes | no\n";
        return 1;
    }

    std::string image_path = argv[1];
    std::string style_str = to_lower(argv[2]);
    std::string colors_str = to_lower(argv[3]);
    int width = 80;
    if (argc >= 5) {
        try {
            width = std::stoi(argv[4]);
            if (width <= 0) width = 80;
        } catch (...) {
            // ignore and use default
        }
    }

    ascii_art::Config cfg;
    cfg.target_width = width;

    if (style_str == "clean" || style_str == "c") {
        cfg.mode = ascii_art::Mode::CLEAN;
    } else if (style_str == "high_fidelity" || style_str == "high" || style_str == "hf") {
        cfg.mode = ascii_art::Mode::HIGH_FIDELITY;
    } else if (style_str == "block" || style_str == "b") {
        cfg.mode = ascii_art::Mode::BLOCK;
    } else {
        std::cerr << "Unknown style: " << argv[2] << "\n";
        return 2;
    }

    if (colors_str == "yes" || colors_str == "y" || colors_str == "true" || colors_str == "1") {
        cfg.use_color = true;
    } else if (colors_str == "no" || colors_str == "n" || colors_str == "false" || colors_str == "0") {
        cfg.use_color = false;
    } else {
        std::cerr << "Unknown colors flag (use yes/no): " << argv[3] << "\n";
        return 3;
    }

    ascii_art::Interpreter interp(cfg);

    try {
        std::string out = interp.convert_from_file(image_path);
        std::cout << out;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 4;
    }

    return 0;
}
