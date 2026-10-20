#include "roster_io.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool save_roster(const std::vector<Mech>& roster, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& mech : roster) {
        // Format: name,hp,attack,armor
        file << mech.get_name() << ","
             << mech.get_hp() << ","
             << mech.get_attack() << ","
             << mech.get_armor() << "\n";
    }

    return true;
}

bool load_roster(const std::string& path, std::vector<Mech>& roster, int& skipped_lines) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Mech> temp_roster;
    int bad_lines = 0;
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty lines or handle basic line reading
        if (line.empty()) {
            bad_lines++;
            continue;
        }

        std::stringstream ss(line);
        std::string name_str, hp_str, attack_str, armor_str, extra_str;

        // Exactly 4 fields separated by commas
        if (!std::getline(ss, name_str, ',') ||
            !std::getline(ss, hp_str, ',') ||
            !std::getline(ss, attack_str, ',') ||
            !std::getline(ss, armor_str, ',')) {
            bad_lines++;
            continue;
        }

        // Check if there are extra fields/trailing characters on the line
        if (ss >> extra_str) {
            bad_lines++;
            continue;
        }

        // Name cannot be empty
        if (name_str.empty()) {
            bad_lines++;
            continue;
        }

        // Parse and validate numeric fields using stringstreams (avoiding std::stoi)
        int hp = 0, attack = -1, armor = -1;
        std::stringstream hp_ss(hp_str);
        std::stringstream att_ss(attack_str);
        std::stringstream arm_ss(armor_str);
        char extra_char;

        // Ensure numbers are whole numbers with nothing after them
        if (!(hp_ss >> hp) || (hp_ss >> extra_char) || hp < 1) {
            bad_lines++;
            continue;
        }
        if (!(att_ss >> attack) || (att_ss >> extra_char) || attack < 0) {
            bad_lines++;
            continue;
        }
        if (!(arm_ss >> armor) || (arm_ss >> extra_char) || armor < 0) {
            bad_lines++;
            continue;
        }

        // If all checks pass, construct the mech and add it to our temporary roster
        temp_roster.emplace_back(name_str, hp, attack, armor);
    }

    // Replace roster and set skipped lines count
    roster = std::move(temp_roster);
    skipped_lines = bad_lines;
    return true;
}

bool append_line(const std::string& path, const std::string& text) {
    // Open in append mode so existing content is kept and file is created if missing
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << text << "\n";
    return true;
}