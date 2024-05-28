#include "FormatOutput.h"

std::shared_ptr<spdlog::logger> LoggerManager::file_logger = nullptr;
std::shared_ptr<spdlog::logger> LoggerManager::regular_file_logger = nullptr;

std::shared_ptr<spdlog::logger> LoggerManager::get_unique_logger()
{
    static std::atomic<int> counter{0};
    std::ostringstream oss;
    oss << "UNIQUE_" << counter++;
    auto logger = spdlog::stdout_color_mt(oss.str());
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    return logger;
}

std::shared_ptr<spdlog::logger>& LoggerManager::getFileLogger(bool isNotRegular)
{
    if (isNotRegular == false) return regular_file_logger;
    else return file_logger;
}

void LoggerManager::initializeFile()
{
    if (file_logger)
    {
        // Видалення попереднього логера, якщо він існує
        spdlog::drop(file_logger->name());
    }

    file_logger = spdlog::basic_logger_mt("global_file_logger", "..\\bin\\global_scenario_log.txt", true);
}

void LoggerManager::initializeRegularFile()
{
    if (regular_file_logger)
    {
        // Видалення попереднього логера, якщо він існує
        spdlog::drop(regular_file_logger->name());
    }

    regular_file_logger = spdlog::basic_logger_mt("regular_file_logger", "..\\bin\\regular_scenario_log.txt", true);
}
