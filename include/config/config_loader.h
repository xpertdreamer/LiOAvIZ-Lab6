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
};

struct ConsoleConfig {
    std::string prompt = "> ";
    std::string welcome_msg = "Console";
    std::unordered_map<std::string, std::string> colors;
    std::vector<CommandConfig> commands;
};

class ConfigLoader {
public:
    static ConsoleConfig load_from_file(const std::string& filename);
    static std::unordered_map<std::string, std::string> load_aliases(const std::string& filename);
private:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif //CONFIG_LOADER_H
