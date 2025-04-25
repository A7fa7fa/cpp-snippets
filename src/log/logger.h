#pragma once
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <chrono>

class Logger {
public:
    enum Level { DEBUG = 1, INFO, WARN, ERROR };

    static Logger& instance() {
        static Logger logger("log.log", Level::INFO); // default filename
        return logger;
    }

    void set_log_file(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mu);
        if (fd != -1) close(fd);
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    }

    void set_log_level(Level log_level) {
        this->log_level = log_level;
    }


    void debug(const std::string& message)  { log(DEBUG, message); }
    void info(const std::string& message)  { log(INFO, message); }
    void warn(const std::string& message)  { log(WARN, message); }
    void error(const std::string& message) { log(ERROR, message); }

private:
    int fd = -1;
    std::mutex mu;
    Level log_level;

    Logger(const std::string& filename, Level log_level ) {
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        this->log_level = log_level;
    }

    ~Logger() {
        if (fd != -1) close(fd);
    }

    // prevent copies
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(Level level, const std::string& message) {
        if (this->log_level > level) {
            return;
        }

        std::lock_guard<std::mutex> lock(mu);
        std::string prefix = level_prefix(level);
        std::string timestamp = current_time();

        std::ostringstream full_message;
        full_message << "[" << timestamp << "] " << prefix << " " << message << "\n";

        std::string output = full_message.str();


        std::cout << output;

        if (fd != -1) {
            write(fd, output.c_str(), output.size());
        }

    }

    std::string level_prefix(Level level) {
        switch (level) {
            case DEBUG: return "[DEBUG]";
            case INFO:  return "[INFO ]";
            case WARN:  return "[WARN ]";
            case ERROR: return "[ERROR]";
            default:    return "[UNKWN]";
        }
    }

    std::string current_time() {

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm tm;
        gmtime_r(&in_time_t, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " UTC";

        return oss.str();
    }

};