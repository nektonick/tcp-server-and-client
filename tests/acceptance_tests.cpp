#include <memory>
#include <doctest/doctest.h>
#include "server/server.h"
#include "client/client.h"

class ServerFixture {
public:
    ServerFixture() {
        server_ = std::make_unique<Server>();
        server_->start();
    }

    virtual ~ServerFixture() = default;

    TcpConnection::port_t testPort = 12345;
    std::unique_ptr<Server> server_;
};

class ClientServerFixture : public ServerFixture {
public:
    ClientServerFixture() : ServerFixture() {
        auto connection = std::make_unique<TcpConnection>("127.0.0.1", testPort);
        client_ = std::make_unique<Client>(std::move(connection));
    }

    std::unique_ptr<Client> client_;
};

TEST_CASE("Server port can be configured") {
}

TEST_CASE_FIXTURE(ClientServerFixture, "Server support info request") {
    const auto response = client_->executeInf();
    CHECK(response.lastCommands.size() == 0);
}

TEST_CASE_FIXTURE(ClientServerFixture, "Server support command request") {
    const auto response = client_->executeCmd("echo test");
    CHECK(response.resultCode == "ok");
}

TEST_CASE_FIXTURE(ClientServerFixture, "Info request returns last 10 commands") {
    const int cmdRequestsCount = 11;
    for (int i = 0; i < cmdRequestsCount; i++) {
        const auto cmd = "echo " + std::to_string(i);
        const auto response = client_->executeCmd(cmd);
        REQUIRE(response.resultCode == "ok");
    }

    const auto response = client_->executeInf();
    REQUIRE(response.lastCommands.size() == cmdRequestsCount);
    for (int i = 0; i < cmdRequestsCount; i++) {
        const auto expectedCmd = "echo " + std::to_string(i);
        REQUIRE(response.lastCommands[i].functionToExecute == expectedCmd);
    }
}

TEST_CASE("Only one command request can be executed at a time") {
}

TEST_CASE("Command request can be queued") {
}

TEST_CASE("Info response does not include non-finished commands") {
}

TEST_CASE("Server creates log file") {
}

TEST_CASE_FIXTURE(ServerFixture, "Multiple clients are supported") {
    const auto client1 = std::make_unique<Client>(std::make_unique<TcpConnection>("127.0.0.1", testPort));
    const auto client2 = std::make_unique<Client>(std::make_unique<TcpConnection>("127.0.0.1", testPort));

    const auto response1 = client1->executeCmd("echo client1");
    REQUIRE(response1.resultCode == "ok");
    const auto response2 = client2->executeCmd("echo client2");
    REQUIRE(response2.resultCode == "ok");
}

TEST_CASE("If client sends request with invalid json, server returns error") {
}

TEST_CASE("If client sends request with invalid type, server returns error") {
}

TEST_CASE("Client print responses to stdout") {
}
