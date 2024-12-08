#include "client.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TcpConnection::TcpConnection(const std::string &host, port_t port)
    : socket_(io_context_) {
    socket_.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
}

IConnection::response_t TcpConnection::send(const std::string &message) {
    boost::asio::write(socket_, boost::asio::buffer(message));

    boost::asio::streambuf buffer;
    boost::asio::read(socket_, buffer, boost::asio::transfer_all());
    std::istream is(&buffer);
    std::string response((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    return response;
}

Client::Client(std::unique_ptr<IConnection> connection) : connection_(std::move(connection)) {
}

void from_json(const json &j, Command &c) {
    j.at("timestamp").get_to(c.timestamp);
    j.at("type").get_to(c.type);
    if (j.contains("function")) {
        c.functionToExecute = j.at("function").get<std::string>();
    }
}

void from_json(const json &j, InfResult &r) {
    j.at("timestamp").get_to(r.timestamp);
    j.at("last_executed_commands").get_to(r.lastCommands);
}

void from_json(const json &j, CmdResult &r) {
    j.at("resultCode").get_to(r.resultCode);
}

InfResult Client::executeInf() {
    const nlohmann::json request = {{"type", "inf"}};
    const auto response = connection_->send(request.dump());
    const auto responseJson = json::parse(response);
    return responseJson.get<InfResult>();
}

CmdResult Client::executeCmd(std::string commandToExecute) {
    const nlohmann::json request = {{"type", "cmd"}, {"function", commandToExecute}};
    const auto response = connection_->send(request.dump());
    const auto responseJson = json::parse(response);
    return responseJson.get<CmdResult>();
}
