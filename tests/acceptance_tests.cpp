#include <doctest/doctest.h>
#include "server/server.h"
#include "client/client.h"

TEST_CASE("Server port can be configured") {
}

TEST_CASE("Server support info request") {
}

TEST_CASE("Server support command request") {
}

TEST_CASE("Info request returns last 10 commands") {
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
}

TEST_CASE("If client sends request with invalid json, server returns error") {
}

TEST_CASE("If client sends request with invalid type, server returns error") {
}

TEST_CASE("Client print responses to stdout") {
}
