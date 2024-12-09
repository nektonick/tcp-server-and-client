#include <iostream>
#include <memory>
#include <doctest/doctest.h>
#include <shared/constants.h>

#include "server/server.h"
#include "client/client.h"

class ConsoleWriter final : public IWriter {
public:
    void write(const std::string &message) override {
        std::cout << message;
    }

    std::vector<std::string> messages_;
};

ILoggerPtr createTestLogger(std::string name) {
    auto writer = std::make_shared<ConsoleWriter>();
    auto logger = std::make_shared<ThreadSafeLogger>(std::move(name), std::move(writer));
    return logger;
}

class ServerFixture {
public:
    ServerFixture() {
        server_ = std::make_unique<Server>(createTestLogger("Server"), testPort);
        server_->startAsync();
    }

    virtual ~ServerFixture() = default;

    port_t testPort = 12345;
    std::unique_ptr<Server> server_;
};

class ClientServerFixture : public ServerFixture {
public:
    ClientServerFixture() : ServerFixture() {
        auto connection = std::make_unique<TcpConnection>("127.0.0.1", testPort);
        client_ = std::make_unique<Client>(createTestLogger("Client"), std::move(connection));
    }

    std::unique_ptr<Client> client_;
};

TEST_CASE_FIXTURE(ClientServerFixture, "Server support info request") {
    const auto response = client_->executeInf();
    CHECK(response.lastCommands.size() == 0);
}

TEST_CASE_FIXTURE(ClientServerFixture, "Server support command request") {
    const auto response = client_->executeCmd("echo test");
    CHECK(response.resultCode == "ok");
}

TEST_CASE_FIXTURE(ClientServerFixture, "Info request returns last commands") {
    const int cmdRequestsCount = COMMANDS_HISTORY_SIZE + 1;
    for (int i = 0; i < cmdRequestsCount; i++) {
        const auto cmd = "echo " + std::to_string(i);
        const auto response = client_->executeCmd(cmd);
        REQUIRE(response.resultCode == "ok");
    }

    const auto response = client_->executeInf();
    REQUIRE(response.lastCommands.size() == COMMANDS_HISTORY_SIZE);
    for (int i = 0; i < response.lastCommands.size(); i++) {
        const auto expectedCmd = "echo " + std::to_string(i + 1);
        REQUIRE(response.lastCommands[i].functionToExecute != std::nullopt);
        REQUIRE(response.lastCommands[i].functionToExecute.value() == expectedCmd);
    }
}

TEST_CASE_FIXTURE(ServerFixture, "Multiple clients are supported") {
    const auto client1 = std::make_unique<Client>(createTestLogger("Client 1"),
                                                  std::make_unique<TcpConnection>("127.0.0.1", testPort));
    const auto client2 = std::make_unique<Client>(createTestLogger("Client 2"),
                                                  std::make_unique<TcpConnection>("127.0.0.1", testPort));

    const auto response1 = client1->executeCmd("echo client1");
    REQUIRE(response1.resultCode == "ok");
    const auto response2 = client2->executeCmd("echo client2");
    REQUIRE(response2.resultCode == "ok");
    const auto response3 = client1->executeCmd("echo client1 again");
    REQUIRE(response3.resultCode == "ok");
    const auto response4 = client2->executeCmd("echo client2 again");
    REQUIRE(response4.resultCode == "ok");

    auto requests = client1->executeInf();
    REQUIRE(requests.lastCommands.size() == 4);
}
