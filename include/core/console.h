#ifndef UNIVERSAL_CONSOLE_H
#define UNIVERSAL_CONSOLE_H

#include <deque>
#include <string>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <vector>
#include <iostream>
#include <ranges>
#include <sstream>

#include "../config/config_loader.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#endif
using namespace std;

class Console {
public:
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;

    Console() : running(false) {
        config.prompt = "> ";
        config.welcome_msg = "Console v0.0.1";
    }

    void register_command(const std::string& name, const CommandHandler &handler,
                         const std::string& description = "",
                         const std::vector<std::string>& parameters = {},
                         const std::string& usage = "") {
        CommandInfo info;
        info.handler = handler;
        info.description = description;
        info.parameters = parameters;
        info.usage = usage.empty() ? build_usage(name, parameters) : usage;
        commands[name] = info;
    }

    void register_alias(const std::string& alias, const std::string& command) {
        aliases[alias] = command;
    }

    void set_config(const ConsoleConfig& newConfig) {
        config = newConfig;
        setup_colors();
    }

    void run() {
        running = true;
        std::string input;

        if (config.clear_screen_on_start) {
            clear_screen();
        }

        std::cout << get_color("info") << config.welcome_msg << reset_color() << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;

        while (running) {
            std::cout << get_color("info") << config.prompt << reset_color();
            std::getline(std::cin, input);

            if (input.empty()) continue;

            add_to_history(input);
            process_input(input);

        }
    }

    void stop() {
        running = false;
        std::cout << get_color("success") << config.exit_msg << reset_color() << std::endl;
    }

    static std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    void print_help() {
        std::cout << get_color("info") << "Available commands:" << reset_color() << std::endl;
        size_t max_name_length = 12;
        for (const auto &name: commands | views::keys) {
            max_name_length = std::max(max_name_length, name.length());
        }

        for (const auto& [name, info] : commands) {
            std::cout << "  " << get_color("success") << std::setw(static_cast<int>(max_name_length))
                      << std::left << name << reset_color() << " - " << info.description;

            if (!info.usage.empty()) {
                std::cout << " " << get_color("warning") << "(" << info.usage << ")" << reset_color();
            }
            std::cout << std::endl;
        }
    }

    void show_command_help(const std::string& command_name) {
        std::string resolved = resolve_command(command_name);

        if (auto it = commands.find(resolved); it != commands.end()) {
            const auto& info = it->second;
            std::cout << get_color("info") << "Command: " << resolved << reset_color() << std::endl;
            std::cout << "  Description: " << info.description << std::endl;
            std::cout << "  Usage: " << get_color("success") << info.usage << reset_color() << std::endl;

            if (!info.parameters.empty()) {
                std::cout << "  Parameters:" << std::endl;
                for (const auto& param : info.parameters) {
                    std::cout << "    - " << param << std::endl;
                }
            }
        } else {
            std::cout << get_color("error") << "Unknown command: " << command_name << reset_color() << std::endl;
        }
    }

    void load_config(const std::string& configPath) {
        config = ConfigLoader::load_from_file(configPath);
        setup_colors();
    }

    void load_aliases(const std::string& aliasPath) {
        auto newAliases = ConfigLoader::load_aliases(aliasPath);
        aliases.insert(newAliases.begin(), newAliases.end());
    }

    std::unordered_map<std::string, std::string> get_aliases() {
        return aliases;
    }

    void clear_screen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    void show_history() {
        std::cout << get_color("info") << "Command history (last " << command_history.size() << " commands):" << reset_color() << std::endl;
        for (size_t i = 0; i < command_history.size(); ++i) {
            std::cout << "  " << (i + 1) << ": " << command_history[i] << std::endl;
        }
    }

private:
    bool running;
    std::deque<std::string> commands_history;
    ConsoleConfig config;
    std::deque<std::string> command_history;

    struct CommandInfo {
        CommandHandler handler;
        std::string description;
        std::vector<std::string> parameters;
        std::string usage;
    };

    std::unordered_map<std::string, CommandInfo> commands;
    std::unordered_map<std::string, std::string> aliases;

    std::string resolve_command(const std::string& input) {
        const auto it = aliases.find(input);
        return it != aliases.end() ? it->second : input;
    }

    void process_input(const std::string& input) {
        auto tokens = tokenize(input);
        if (tokens.empty()) return;

        std::string commandName = tokens[0];

        if (commandName == "exit" || commandName == "quit") {
            stop();
            return;
        }

        if (commandName == "help") {
            if (tokens.size() > 1) {
                show_command_help(tokens[1]);
            } else {
                print_help();
            }
            return;
        }

        if (commandName == "clear") {
            clear_screen();
            return;
        }

        if (commandName == "history") {
            show_history();
            return;
        }

        std::string resolvedCommand = resolve_command(commandName);
        if (resolvedCommand != commandName) {
            tokens[0] = resolvedCommand;
        }

        commandName = resolvedCommand;

        if (const auto it = commands.find(commandName); it != commands.end()) {
            try {
                const std::vector<std::string> args(tokens.begin() + 1, tokens.end());
                it->second.handler(args);
            } catch (const std::exception& e) {
                std::cout << get_color("error") << "Error executing command: " << e.what() << reset_color() << std::endl;
            }
        } else {
            std::cout << get_color("error") << config.unknown_msg << ": " << commandName << reset_color() << std::endl;
            if (config.show_help_on_unknown) {
                std::cout << "Type 'help' for available commands" << std::endl;
            }
        }
    }

    void add_to_history(const std::string& command) {
        command_history.push_front(command);
        if (command_history.size() > static_cast<size_t>(config.history_size)) {
            command_history.pop_back();
        }
    }

    void setup_colors() {
        if (!config.colors_enabled) {
            config.colors.clear();
            return;
        }

#ifdef _WIN32
        // Windows color setup
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        // Enable virtual terminal processing on Windows 10+
        DWORD mode = 0;
        GetConsoleMode(hConsole, &mode);
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

        // ANSI color codes
        config.colors = {
            {"reset", "\033[0m"},
            {"black", "\033[30m"},
            {"red", "\033[31m"},
            {"green", "\033[32m"},
            {"yellow", "\033[33m"},
            {"blue", "\033[34m"},
            {"magenta", "\033[35m"},
            {"cyan", "\033[36m"},
            {"white", "\033[37m"},
            {"bright_red", "\033[91m"},
            {"bright_green", "\033[92m"},
            {"bright_yellow", "\033[93m"},
            {"bright_blue", "\033[94m"}
        };
    }

    std::string get_color(const std::string& type) {
        if (!config.colors_enabled) return "";

        std::string color_name;
        if (type == "error") color_name = config.error_color;
        else if (type == "success") color_name = config.success_color;
        else if (type == "warning") color_name = config.warning_color;
        else if (type == "info") color_name = config.info_color;
        else color_name = "reset";

        const auto it = config.colors.find(color_name);
        return it != config.colors.end() ? it->second : config.colors.at("reset");
    }

    std::string reset_color() {
        return config.colors_enabled ? config.colors.at("reset") : "";
    }

    std::string build_usage(const std::string& command, const std::vector<std::string>& parameters) {
        std::string usage = command;
        for (const auto& param : parameters) {
            usage += " <" + param + ">";
        }
        return usage;
    }
};

#endif