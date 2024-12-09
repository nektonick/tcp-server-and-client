#include "client.h"
#include "../shared/constants.h"

TcpConnection::TcpConnection(const std::string &host, port_t port)
    : socket_(io_context_) {
    socket_.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
}

IConnection::response_t TcpConnection::send(const std::string &message) {
    boost::asio::write(socket_, boost::asio::buffer(message + END_OF_TCP_MESSAGE));

    boost::asio::streambuf buffer;
    boost::system::error_code errorCode;
    boost::asio::read_until(socket_, buffer, END_OF_TCP_MESSAGE, errorCode);
    if (errorCode) {
        throw std::runtime_error("TcpConnection::send() failed: " + errorCode.message());
    }
    std::istream is(&buffer);
    std::string response((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    response.pop_back();
    return response;
}

std::future<IConnection::response_t> TcpConnection::sendAsync(const std::string &message) {
    return std::async(std::launch::async, [this, &message] { return send(message); });
}

Client::Client(ILoggerPtr logger, std::unique_ptr<IConnection> connection)
    : logger_(std::move(logger)),
      connection_(std::move(connection)) {
}

InfResult Client::executeInf() {
    logger_->info("Start executing Inf command");
    const nlohmann::json request = {{"type", "inf"}};
    const auto response = connection_->send(request.dump());
    const auto responseJson = json::parse(response);
    logger_->info("Finish executing Inf command. Response:" + response);
    return responseJson.get<InfResult>();
}

CmdResult Client::executeCmd(std::string commandToExecute) {
    logger_->info("Start executing Cmd command, Command:" + commandToExecute);
    const nlohmann::json request = {{"type", "cmd"}, {"function", commandToExecute}};
    const auto response = connection_->send(request.dump());
    const auto responseJson = json::parse(response);
    logger_->info("Finish executing Cmd command. Response:" + response);
    return responseJson.get<CmdResult>();
}
