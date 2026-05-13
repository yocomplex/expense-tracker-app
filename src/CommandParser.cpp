#include "CommandParser.h"

#include <stdexcept>

ParsedCommand CommandParser::parse(int argc, char* argv[]) {
    if (argc < 2) {
        throw std::invalid_argument("No command provided.");
    }

    ParsedCommand command;
    command.action = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string token = argv[i];

        if (token.rfind("--", 0) == 0) {
            std::string optionName = token.substr(2);

            if (optionName.empty()) {
                throw std::invalid_argument("Invalid option name.");
            }

            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for option --" + optionName);
            }

            std::string value = argv[++i];
            command.options[optionName] = value;
        } else {
            throw std::invalid_argument("Unexpected argument: " + token);
        }
    }

    return command;
}

bool CommandParser::hasOption(const ParsedCommand& command, const std::string& name) {
    return command.options.find(name) != command.options.end();
}

std::optional<std::string> CommandParser::getOption(const ParsedCommand& command, const std::string& name) {
    auto it = command.options.find(name);
    if (it == command.options.end()) {
        return std::nullopt;
    }

    return it->second;
}

int CommandParser::getRequiredInt(const ParsedCommand& command, const std::string& name) {
    auto value = getOption(command, name);
    if (!value.has_value()) {
        throw std::invalid_argument("Missing required option --" + name);
    }

    try {
        return std::stoi(*value);
    } catch (...) {
        throw std::invalid_argument("Option --" + name + " must be an integer.");
    }
}

double CommandParser::getRequiredDouble(const ParsedCommand& command, const std::string& name) {
    auto value = getOption(command, name);
    if (!value.has_value()) {
        throw std::invalid_argument("Missing required option --" + name);
    }

    try {
        return std::stod(*value);
    } catch (...) {
        throw std::invalid_argument("Option --" + name + " must be a number.");
    }
}

std::string CommandParser::getRequiredString(const ParsedCommand& command, const std::string& name) {
    auto value = getOption(command, name);
    if (!value.has_value()) {
        throw std::invalid_argument("Missing required option --" + name);
    }

    return *value;
}