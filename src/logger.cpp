#include "logger.h"

FileWriter::FileWriter(std::string &&targetFile): targetFile_(std::move(targetFile)) {
}

void FileWriter::write(const std::string &message) {
    if (std::ofstream outputStream(targetFile_, std::ios::app); outputStream.is_open()) {
        outputStream << message;
    }
}

ThreadSafeLogger::ThreadSafeLogger(std::string name, std::shared_ptr<IWriter> writer): name_(std::move(name)),
    writer_(std::move(writer)) {
}

void ThreadSafeLogger::info(const std::string &message) {
    log("INFO", message);
}

void ThreadSafeLogger::error(const std::string &message) {
    log("ERROR", message);
}

void ThreadSafeLogger::log(const std::string &level, const std::string &message) {
    std::scoped_lock lock(mtx_);
    std::stringstream ss;
    ss << "[" << getCurrentTime() << "ms] [" << level << "] " << name_ << ": " << message << std::endl;
    writer_->write(ss.str());
}

std::string ThreadSafeLogger::getCurrentTime() {
    const auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    const auto currentTimeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime);
    return std::to_string(currentTimeInMs.count());
}
