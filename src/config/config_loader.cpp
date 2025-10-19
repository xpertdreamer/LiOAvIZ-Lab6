//
// Created by IWOFLEUR on 13.10.2025.
//

#include "../../include/config/config_loader.h"

#include <fstream>
#include <sstream>

std::string ConfigLoader::trim(const std::string &str) {
    const size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) return str;
    const size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> ConfigLoader::split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

std::unordered_map<std::string, std::string> ConfigLoader::load_aliases(const std::string &filename) {
    std::unordered_map<std::string, std::string> aliases;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (auto parts= split(line, '='); parts.size() == 2) aliases[parts[0]] = parts[1];
    }

    return aliases;
}

ConsoleConfig ConfigLoader::load_from_file(const std::string &filename) {
    ConsoleConfig config;
    std::ifstream file(filename);
    std::string line;

    CommandConfig command;
    bool isCommandSection = false;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[' && line[line.size() - 1] == ']') {
            if (isCommandSection && !command.name.empty()) {
                config.commands.push_back(command);
                command = CommandConfig();
            }

            std::string section = line.substr(1, line.size() - 2);
            isCommandSection = (section == "command");
            continue;
        }

        auto parts = split(line, '=');
        if (parts.size() < 2) continue;

        const std::string& key = parts[0];
        const std::string& value = parts[1];

        if (isCommandSection) {
            if (key == "name") command.name = value;
            else if (key == "description") command.description = value;
            else if (key == "aliases") {
                auto aliases = split(value, ',');
                command.aliases = aliases;
            }
        } else {
            if (key == "prompt") config.prompt = value;
            else if (key == "welcome_msg") config.welcome_msg = value;
        }
    }

    if (isCommandSection && !command.name.empty()) config.commands.emplace_back();

    return config;
}

