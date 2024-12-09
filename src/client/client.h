#pragma once
#include <memory>
#include <string>
#include <boost/asio.hpp>

#include "../logger.h"
#include "../shared/structs.h"
#include "../shared/type_aliases.h"


class IConnection {
public:
    virtual ~IConnection() = default;

    using response_t = std::string;

    virtual response_t send(const std::string &message) = 0;

    virtual std::future<response_t> sendAsync(const std::string &message) = 0;
};

class TcpConnection final : public IConnection {
public:
    TcpConnection(const std::string &host, port_t port);

    response_t send(const std::string &message) override;

    std::future<response_t> sendAsync(const std::string &message) override;

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};

class Client {
public:
    explicit Client(ILoggerPtr logger, std::unique_ptr<IConnection> connection);

    InfResult executeInf();

    CmdResult executeCmd(std::string commandToExecute);

private:
    ILoggerPtr logger_;
    std::unique_ptr<IConnection> connection_;
};
