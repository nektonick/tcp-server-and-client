#pragma once
#include <deque>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class CommandType {
    Inf,
    Cmd,
};

struct Command {
    std::string timestamp;
    CommandType type;
    std::optional<std::string> functionToExecute;
};

struct InfResult {
    std::string timestamp;
    std::deque<Command> lastCommands;
};

struct CmdResult {
    std::string resultCode;
};


void from_json(const json &j, Command &c);

void to_json(json &j, const Command &c);

void from_json(const json &j, InfResult &r);

void to_json(json &j, const InfResult &r);

void from_json(const json &j, CmdResult &r);

void to_json(json &j, const CmdResult &r);
