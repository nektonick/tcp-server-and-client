#include <memory>
#include <vector>
#include <doctest/doctest.h>
#include "logger.h"

class MockWriter final : public IWriter {
public:
    void write(const std::string &message) override {
        messages_.push_back(message);
    }

    std::vector<std::string> messages_;
};

TEST_CASE("Logger") {
    std::string loggerName = "TestLogger";
    auto writer = std::make_shared<MockWriter>();
    auto logger = std::make_unique<ThreadSafeLogger>(loggerName, writer);
    REQUIRE(writer->messages_.size() == 0);

    SUBCASE("can log single info message") {
        const auto testMessage = "Test message 1";
        logger->info(testMessage);

        REQUIRE(writer->messages_.size() == 1);
        auto loggedMessage = writer->messages_[0];
        CHECK_MESSAGE(loggedMessage.find(loggerName) != std::string::npos, "loggedMessage: " + loggedMessage);
        CHECK_MESSAGE(loggedMessage.find("INFO") != std::string::npos, "loggedMessage: " + loggedMessage);
        CHECK_MESSAGE(loggedMessage.find(testMessage) != std::string::npos, "loggedMessage: " + loggedMessage);
    }

    SUBCASE("can log multiple info messages") {
        const int messagesCount = 5;
        const std::string messageBase = "Test message ";
        for (int i = 0; i < messagesCount; ++i) {
            const auto testMessage = "Test message " + std::to_string(i);
            logger->info(testMessage);
        }

        REQUIRE(writer->messages_.size() == messagesCount);

        for (int i = 0; i < messagesCount; ++i) {
            const auto expectedMessage = "Test message " + std::to_string(i);
            auto loggedMessage = writer->messages_[i];
            CHECK_MESSAGE(loggedMessage.find(loggerName) != std::string::npos, "loggedMessage: " + loggedMessage);
            CHECK_MESSAGE(loggedMessage.find("INFO") != std::string::npos, "loggedMessage: " + loggedMessage);
            CHECK_MESSAGE(loggedMessage.find(expectedMessage) != std::string::npos, "loggedMessage: " + loggedMessage);
        }
    }
}
