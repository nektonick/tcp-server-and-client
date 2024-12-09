#include "server.h"

#include <iomanip>
#include <nlohmann/json.hpp>

void LastCommands::add(Command command) {
    std::scoped_lock lck(mtx_);
    commands_.push_back(std::move(command));
    if (commands_.size() > COMMANDS_HISTORY_SIZE) {
        commands_.pop_front();
    }
    assert(commands_.size() <= COMMANDS_HISTORY_SIZE);
}

std::deque<Command> LastCommands::get() {
    std::scoped_lock lck(mtx_);
    return commands_;
}

Server::Server(ILoggerPtr logger, port_t port)
    : logger_(std::move(logger)),
      acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {
}

Server::~Server() {
    stop();
    clientThreads.join();
    if (mainThread_.joinable()) {
        mainThread_.join();
    }
}

void Server::startAsync() {
    logger_->info("Starting server");
    acceptConnection();
    logger_->info("Server started");
    mainThread_ = std::thread([this] {
        io_context_.run();
    });
}

void Server::start() {
    logger_->info("Starting server");
    acceptConnection();
    logger_->info("Server started");
    io_context_.run();
}

void Server::stop() {
    logger_->info("Stopping server");
    shutdownRequired_ = true;
    io_context_.stop();
    logger_->info("Server stopped");
}

void Server::acceptConnection() {
    logger_->info("Waiting for new connection");
    auto socket = std::make_shared<tcp::socket>(io_context_);
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        logger_->info("Connection established");
        if (!ec) {
            logger_->info("Going to handle client in separate thread");
            boost::asio::post(clientThreads, [this, socket]() {
                try {
                    handleClient(*socket);
                } catch (const std::exception &e) {
                    logger_->error("Error while handling client: " + std::string(e.what()));
                }
            });
        }
        if (!shutdownRequired_) {
            acceptConnection();
        }
    });
}

void Server::handleClient(tcp::socket &socket) {
    logger_->info("Handling client");

    boost::asio::streambuf buffer;
    logger_->info("Waiting for request");
    boost::system::error_code errorCode;
    boost::asio::read_until(socket, buffer, END_OF_TCP_MESSAGE, errorCode);
    if (errorCode) {
        if (errorCode == boost::asio::error::eof) {
            logger_->info("Client disconnected");
            socket.close();
            return;
        } else {
            logger_->error("Unknown error: " + errorCode.message());
            socket.close();
            return;
        }
    }
    std::istream is(&buffer);
    std::string request((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    request.pop_back(); // Removing END_OF_TCP_MESSAGE symbol
    logger_->info("Request received: " + request);

    logger_->info("Start processing request");
    std::string response = processRequest(request);
    logger_->info("Finish processing request");

    logger_->info("Sending response: " + response);
    boost::asio::write(socket, boost::asio::buffer(response + END_OF_TCP_MESSAGE));
    logger_->info("Response sent");

    if (!shutdownRequired_) {
        handleClient(socket);
    }
}

std::string getTimestamp() {
    // TODO: replace with std::format after updating to gcc-13
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
    auto str = oss.str();
    return str;
}

std::string Server::processRequest(const std::string &request) {
    // TODO: move request processing logic to separate class
    nlohmann::json json_request;
    try {
        json_request = nlohmann::json::parse(request);
    } catch (const nlohmann::json::parse_error &e) {
        logger_->error("Invalid JSON request: " + std::string(e.what()));
        return nlohmann::json{{"resultCode", "Invalid json"}}.dump();
    }

    if (!json_request.contains("type")) {
        logger_->error("Missing 'type' field");
        return nlohmann::json{{"resultCode", "Missing 'type' field"}}.dump();
    }

    if (json_request["type"] == "inf") {
        InfResult infResult{getTimestamp(), lastCommands_.get()};
        lastCommands_.add(Command{getTimestamp(), CommandType::Inf, std::nullopt});
        return nlohmann::json(infResult).dump();
    } else if (json_request["type"] == "cmd") {
        if (!json_request.contains("function")) {
            logger_->error("Missing 'function' field");
            return nlohmann::json{{"resultCode", "Missing 'function' field"}}.dump();
        }

        std::scoped_lock lck(commandExecutionMtx_);
        lastCommands_.add(Command{getTimestamp(), CommandType::Cmd, json_request["function"]});
        return nlohmann::json({{"resultCode", "ok"}}).dump();
    } else {
        logger_->error("Unsupported 'type'");
        return nlohmann::json{{"resultCode", "Unsupported 'type'"}}.dump();
    }
}
