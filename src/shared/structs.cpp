#include "structs.h"

void from_json(const json &j, Command &c) {
    j.at("timestamp").get_to(c.timestamp);
    j.at("type").get_to(c.type);
    if (j.contains("function")) {
        c.functionToExecute = j.at("function").get<std::string>();
    }
}

void to_json(json &j, const Command &c) {
    j = json{{"timestamp", c.timestamp}, {"type", c.type}};
    if (c.functionToExecute) {
        j["function"] = c.functionToExecute.value();
    }
}

void from_json(const json &j, InfResult &r) {
    j.at("timestamp").get_to(r.timestamp);
    j.at("last_executed_commands").get_to(r.lastCommands);
}

void to_json(json &j, const InfResult &r) {
    j = json{{"timestamp", r.timestamp}, {"last_executed_commands", r.lastCommands}};
}

void from_json(const json &j, CmdResult &r) {
    j.at("resultCode").get_to(r.resultCode);
}

void to_json(json &j, const CmdResult &r) {
    j = json{{"resultCode", r.resultCode}};
}
