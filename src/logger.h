#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <memory>
#include <sstream>

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void info(const std::string &message) = 0;

    virtual void error(const std::string &message) = 0;
};

using ILoggerPtr = std::shared_ptr<ILogger>;

class IWriter {
public:
    virtual ~IWriter() = default;

    virtual void write(const std::string &message) = 0;
};

class FileWriter final : public IWriter {
public:
    explicit FileWriter(std::string &&targetFile);

    void write(const std::string &message) override;

private:
    std::string targetFile_;
};

class ThreadSafeLogger final : public ILogger {
public:
    ThreadSafeLogger(std::string name, std::shared_ptr<IWriter> writer);

    void info(const std::string &message) override;

    void error(const std::string &message) override;

private:
    void log(const std::string &level, const std::string &message);

    static std::string getCurrentTime();

    std::mutex mtx_;
    std::string name_;
    std::shared_ptr<IWriter> writer_;
};
