//
// Created by IWOFLEUR on 13.10.2025.
//

#include "../../include/config/config_loader.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

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

std::string ConfigLoader::join(const std::vector<std::string> &tokens, const std::string &delimiter) {
    std::string result;
    for (size_t i = 0; i < tokens.size(); i++) {
        result += tokens[i];
        if (i != tokens.size() - 1) result += delimiter;
    }
    return result;
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

bool ConfigLoader::parse_bool(const std::string &str) {
    std::string lower = str;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
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
            else if (key == "parameters") {
                auto params = split(value, ',');
                command.parameters = params;
            }
            else if (key == "usage") command.usage = value;
        } else {
            if (key == "prompt") config.prompt = value;
            else if (key == "welcome_msg") config.welcome_msg = value;
            else if (key == "exit_msg") config.exit_msg = value;
            else if (key == "unknown_cmd_msg") config.unknown_msg = value;
            else if (key == "error_color") config.error_color = value;
            else if (key == "success_color") config.success_color = value;
            else if (key == "warning_color") config.warning_color = value;
            else if (key == "info_color") config.info_color = value;
            else if (key == "enable_colors") config.colors_enabled = parse_bool(value);
            else if (key == "show_help_on_unknown") config.show_help_on_unknown = parse_bool(value);
            else if (key == "clear_screen_on_start") config.clear_screen_on_start = parse_bool(value);
            else if (key == "history_size") config.history_size = std::stoi(value);
        }
    }

    if (isCommandSection && !command.name.empty()) config.commands.emplace_back();

    return config;
}

bool ConfigLoader::save_config(const ConsoleConfig &config, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "# Console Configuration\n";
    file << "prompt = " << config.prompt << "\n";
    file << "welcome_msg = " << config.welcome_msg << "\n";
    file << "exit_msg = " << config.exit_msg << "\n";
    file << "unknown_cmd_msg = " << config.unknown_msg << "\n";
    file << "enable_colors = " << (config.colors_enabled ? "true" : "false") << "\n";
    file << "show_help_on_unknown = " << (config.show_help_on_unknown ? "true" : "false") << "\n";
    file << "clear_screen_on_start = " << (config.clear_screen_on_start ? "true" : "false") << "\n";
    file << "history_size = " << config.history_size << "\n\n";

    for (const auto& cmd : config.commands) {
        file << "[command]\n";
        file << "name = " << cmd.name << "\n";
        file << "description = " << cmd.description << "\n";
        if (!cmd.aliases.empty()) {
            file << "aliases = " << join(cmd.aliases, ",") << "\n";
        }
        if (!cmd.parameters.empty()) {
            file << "parameters = " << join(cmd.parameters, ",") << "\n";
        }
        if (!cmd.usage.empty()) {
            file << "usage = " << cmd.usage << "\n";
        }
        file << "\n";
    }

    return true;
}
