#include <iostream>

#include "server.h"

int main(int argc, char *argv[]) {
    port_t port = 12345; // TODO: get from config file or args
    auto writer = std::make_shared<FileWriter>("./server.log");
    auto logger = std::make_shared<ThreadSafeLogger>("Server", std::move(writer));
    Server server(logger, port);
    try {
        // TODO: run startAsync and wait for SIG_INT
        server.start();
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
