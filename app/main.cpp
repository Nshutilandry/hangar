#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "battle_sim.h"
#include "mech.h"
#include "roster_io.h"

#ifndef HANGAR_DATA_DIR
#define HANGAR_DATA_DIR "data"
#endif

namespace {

const std::string roster_path = std::string(HANGAR_DATA_DIR) + "/roster.csv";
const std::string battle_log_path = std::string(HANGAR_DATA_DIR) + "/battle_log.txt";
const std::string graveyard_path = std::string(HANGAR_DATA_DIR) + "/graveyard.txt";


bool read_int(const std::string& prompt, int low, int high, int& result) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) {
            return false;
        }
        std::istringstream in(line);
        int value = 0;
        if (in >> value && (in >> std::ws).eof() && value >= low && value <= high) {
            result = value;
            return true;
        }
        std::cout << "Enter a number from " << low << " to " << high << ".\n";
    }
}

void print_menu() {
    std::cout << "\n=== MECH HANGAR ===\n"
              << "1. List mechs\n"
              << "2. Salvage a new mech\n"
              << "3. Save roster\n"
              << "4. Load roster\n"
              << "5. Training duel\n"
              << "6. Quit\n";
}

void print_roster(const std::vector<Mech>& roster) {
    if (roster.empty()) {
        std::cout << "The hangar is empty.\n";
        return;
    }
    for (std::size_t i = 0; i < roster.size(); ++i) {
        std::cout << (i + 1) << ". ";
        roster[i].print();
    }
}

}  // namespace

int main() {
    std::vector<Mech> roster;
    roster.push_back(make_random_mech());
    roster.push_back(make_random_mech());

    // STRETCH: try to load roster_path here, before the menu starts.
    int startup_skipped = 0;
    load_roster(roster_path, roster, startup_skipped);

    while (true) {
        print_menu();
        int choice = 0;
        if (!read_int("> ", 1, 6, choice)) {
            break;
        }

        if (choice == 1) {
            print_roster(roster);

        } else if (choice == 2) {
            roster.push_back(make_random_mech());
            std::cout << "Salvaged: ";
            roster.back().print();

        } else if (choice == 3) {
            if (save_roster(roster, roster_path)) {
                std::cout << "Roster saved.\n";
            } else {
                std::cout << "Error: Could not save roster to " << roster_path << ".\n";
            }

        } else if (choice == 4) {
            int skipped_lines = 0;
            if (load_roster(roster_path, roster, skipped_lines)) {
                std::cout << "Loaded " << roster.size() << " mechs (" << skipped_lines << " lines skipped).\n";
            } else {
                std::cout << "Error: Could not open roster file " << roster_path << ".\n";
            }

        } else if (choice == 5) {
            if (roster.empty()) {
                std::cout << "No mechs to send into the arena.\n";
                continue;
            }
            print_roster(roster);
            int index = 0;
            if (!read_int("Pick a mech: ", 1, static_cast<int>(roster.size()), index)) {
                break;
            }
            Mech& fighter = roster[static_cast<std::size_t>(index - 1)];
            Mech challenger = make_random_mech();
            std::cout << "Challenger: ";
            challenger.print();

            std::string fighter_name = fighter.get_name();

            DuelResult result = simulate_duel(fighter, challenger);
            std::cout << result.summary << '\n';

            // Checkpoint 4a: append timestamp and result summary to battle log
            append_line(battle_log_path, timestamp() + " " + result.summary);

            // Checkpoint 4b: if the fighter was destroyed, record in graveyard and remove from roster
            if (fighter.get_hp() <= 0) {
                std::string death_record = fighter_name + " was destroyed by " + challenger.get_name();
                append_line(graveyard_path, death_record);
                roster.erase(roster.begin() + (index - 1));
            }

        } else {
            // STRETCH: ask whether to save the roster before quitting.
            char save_choice = 'n';
            std::cout << "Save roster before quitting? (y/n): ";
            if (std::cin >> save_choice && (save_choice == 'y' || save_choice == 'Y')) {
                if (save_roster(roster, roster_path)) {
                    std::cout << "Roster saved.\n";
                } else {
                    std::cout << "Error: Could not save roster.\n";
                }
            }
            break;
        }
    }

    std::cout << "Hangar closed.\n";
    return 0;
}