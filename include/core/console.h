#ifndef UNIVERSAL_CONSOLE_H
#define UNIVERSAL_CONSOLE_H

#include <string>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <vector>
#include <iostream>
#include <sstream>

#include "../config/config_loader.h"

class Console {
public:
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;
    
    Console() : running(false), colorsEnabled(true) {
        config.prompt = "> ";
        config.welcome_msg = "Console";
    }

    void register_command(const std::string& name, const CommandHandler &handler, const std::string& description = "") {
        commands[name] = {handler, description};
    }
    
    void register_alias(const std::string& alias, const std::string& command) {
        aliases[alias] = command;
    }

    void set_config(const ConsoleConfig& newConfig) {
        config = newConfig;
    }
    
    void run() {
        running = true;
        std::string input;

        std::cout << config.welcome_msg << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        
        while (running) {
            std::cout << config.prompt;
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            auto tokens = tokenize(input);
            std::string commandName = tokens[0];
            
            if (commandName == "exit" || commandName == "quit") {
                stop();
                continue;
            }
            
            if (commandName == "help") {
                print_help();
                continue;
            }
            
            std::string resolvedCommand = resolve_command(commandName);
            if (resolvedCommand != commandName) {
                tokens[0] = resolvedCommand;
            }
            
            commandName = resolvedCommand;

            if (auto it = commands.find(commandName); it != commands.end()) {
                try {
                    std::vector args(tokens.begin() + 1, tokens.end());
                    it->second.handler(args);
                } catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << std::endl;
                }
            } else {
                std::cout << "Unknown command: " << commandName << std::endl;
                std::cout << "Type 'help' for available commands" << std::endl;
            }
        }
    }
    
    void stop() {
        running = false;
        std::cout << "Goodbye!" << std::endl;
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
        std::cout << "Available commands:" << std::endl;
        for (const auto& [name, info] : commands) {
            std::cout << "  " << setw(12) << name << " - " << info.description << std::endl;
        }
        std::cout << "  " << setw(12) << "exit, quit" << " - "  << "Exit console" << std::endl;
    }

    void load_config(const std::string& configPath) {
        config = ConfigLoader::load_from_file(configPath);
    }

    void load_aliases(const std::string& aliasPath) {
        auto newAliases = ConfigLoader::load_aliases(aliasPath);
        aliases.insert(newAliases.begin(), newAliases.end());
    }

    std::unordered_map<std::string, std::string> get_aliases() {
        return aliases;
    }
private:
    bool running;
    bool colorsEnabled;
    ConsoleConfig config;
    
    struct CommandInfo {
        CommandHandler handler;
        std::string description;
    };
    
    std::unordered_map<std::string, CommandInfo> commands;
    std::unordered_map<std::string, std::string> aliases;
    
    std::string resolve_command(const std::string& input) {
        const auto it = aliases.find(input);
        return it != aliases.end() ? it->second : input;
    }
};

#endif