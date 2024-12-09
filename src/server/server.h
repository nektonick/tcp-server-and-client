#pragma once
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <deque>
#include <thread>

#include "../logger.h"
#include "../shared/structs.h"
#include "../shared/constants.h"
#include "../shared/type_aliases.h"

using boost::asio::ip::tcp;

class LastCommands {
public:
    void add(Command command);

    std::deque<Command> get();

private:
    std::mutex mtx_;
    std::deque<Command> commands_;
};

class Server {
public:
    explicit Server(ILoggerPtr logger, port_t port);

    ~Server();

    void startAsync();

    void start();

    void stop();

private:
    void acceptConnection();

    void handleClient(tcp::socket &socket);

    std::string processRequest(const std::string &request);

    ILoggerPtr logger_;
    std::thread mainThread_;
    boost::asio::thread_pool clientThreads{CLIENT_THREAD_POOL_SIZE};

    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;

    std::mutex commandExecutionMtx_;
    LastCommands lastCommands_;

    std::atomic_bool shutdownRequired_ = false;
};
