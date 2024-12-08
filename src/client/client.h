#pragma once
#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <boost/asio.hpp>

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

class IConnection {
public:
    virtual ~IConnection() = default;

    using response_t = std::string;

    virtual response_t send(const std::string &message) = 0;
};

class TcpConnection final : public IConnection {
public:
    using port_t = unsigned short;

    TcpConnection(const std::string &host, port_t port);

    response_t send(const std::string &message) override;

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};

class Client {
public:
    explicit Client(std::unique_ptr<IConnection> connection);

    InfResult executeInf();

    CmdResult executeCmd(std::string commandToExecute);

private:
    std::unique_ptr<IConnection> connection_;
};
