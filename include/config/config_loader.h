//
// Created by IWOFLEUR on 13.10.2025.
//

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <unordered_map>
#include <vector>

struct CommandConfig {
    std::string name;
    std::string description;
    std::vector<std::string> aliases;
    std::vector<std::string> parameters;
    std::string usage;
};

struct ConsoleConfig {
    std::string prompt = "> ";
    std::string welcome_msg = "Console";
    std::string exit_msg = "Exit";
    std::string unknown_msg = "Unknown";

    std::string error_color = "red";
    std::string success_color = "green";
    std::string warning_color = "yellow";
    std::string info_color = "blue";

    bool colors_enabled = true;
    bool show_help_on_unknown = true;
    bool clear_screen_on_start = false;
    int history_size = 100;

    std::unordered_map<std::string, std::string> colors;
    std::vector<CommandConfig> commands;
};

class ConfigLoader {
public:
    static ConsoleConfig load_from_file(const std::string& filename);
    static std::unordered_map<std::string, std::string> load_aliases(const std::string& filename);
    static bool save_config(const ConsoleConfig& config, const std::string& filename);
private:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);
    static bool parse_bool(const std::string& str);
};

#endif //CONFIG_LOADER_H
