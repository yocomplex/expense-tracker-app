#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct ParsedCommand {
    std::string action;
    std::unordered_map<std::string, std::string> options;
};

class CommandParser {
public:
    static ParsedCommand parse(int argc, char* argv[]);
    static bool hasOption(const ParsedCommand& command, const std::string& name);
    static std::optional<std::string> getOption(const ParsedCommand& command, const std::string& name);
    static int getRequiredInt(const ParsedCommand& command, const std::string& name);
    static double getRequiredDouble(const ParsedCommand& command, const std::string& name);
    static std::string getRequiredString(const ParsedCommand& command, const std::string& name);
};