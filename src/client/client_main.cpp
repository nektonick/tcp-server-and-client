#include <iostream>

#include "client.h"

int main(int argc, char *argv[]) {
    port_t port = 12345; // TODO: get from config file or args
    auto writer = std::make_shared<FileWriter>("./client.log");
    auto logger = std::make_shared<ThreadSafeLogger>("Client", std::move(writer));

    std::unique_ptr<Client> client;
    try {
        auto connection = std::make_unique<TcpConnection>("127.0.0.1", port);
        client = std::make_unique<Client>(logger, std::move(connection));
    } catch (const std::exception &e) {
        std::cerr << "Can't create client: " << e.what() << std::endl;
        return -1;
    }

    auto printMenu = [] {
        std::cout << "0 - exit\n"
                "1 - info\n"
                "2 - cmd\n";
    };

    printMenu();
    int input;
    std::cin >> input;

    try {
        while (input != 0) {
            if (input == 1) {
                auto res = client->executeInf();
                // TODO: create print function for InfResult
                std::cout << "Last commands: " << nlohmann::json(res).dump() << std::endl;
            } else if (input == 2) {
                std::cout << "Enter command: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                std::string command;
                std::getline(std::cin, command);
                auto res = client->executeCmd(command);
                std::cout << "result: " << res.resultCode << std::endl;
            } else {
                std::cout << "Invalid input" << std::endl;
            }

            printMenu();
            std::cin >> input;
        }
    } catch (const std::exception &e) {
        // TODO: better error handling
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
