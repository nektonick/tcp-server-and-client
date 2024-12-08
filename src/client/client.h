#pragma once
#include <string>
#include <optional>
#include <vector>

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
    std::vector<Command> lastCommands;
};

struct CmdResult {
    std::string resultCode;
};

class Client {
public:
    void connect();

    void disconnect();

    InfResult executeInf();

    CmdResult executeCmd(std::string commandToExecute);
};
