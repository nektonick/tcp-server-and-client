#include <memory>
#include <doctest/doctest.h>
#include "server/server.h"
#include "client/client.h"

TEST_CASE("Server port can be configured") {
}

TEST_CASE("Server support info request") {
    const auto server = std::make_unique<Server>();
    const auto client = std::make_unique<Client>();
    server->start();
    client->connect();
    const auto response = client->executeInf();
    CHECK(response.lastCommands.size() == 0);
}

TEST_CASE("Server support command request") {
    const auto server = std::make_unique<Server>();
    const auto client = std::make_unique<Client>();
    server->start();
    client->connect();
    const auto response = client->executeCmd("echo test");
    CHECK(response.resultCode == "ok");
}

TEST_CASE("Info request returns last 10 commands") {
    const auto server = std::make_unique<Server>();
    const auto client = std::make_unique<Client>();
    server->start();
    client->connect();
    const int cmdRequestsCount = 11;
    for (int i = 0; i < cmdRequestsCount; i++) {
        const auto cmd = "echo " + std::to_string(i);
        const auto response = client->executeCmd(cmd);
        REQUIRE(response.resultCode == "ok");
    }

    const auto response = client->executeInf();
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

TEST_CASE("Multiple clients are supported") {
    const auto server = std::make_unique<Server>();
    const auto client1 = std::make_unique<Client>();
    const auto client2 = std::make_unique<Client>();
    server->start();
    client1->connect();
    client2->connect();

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
