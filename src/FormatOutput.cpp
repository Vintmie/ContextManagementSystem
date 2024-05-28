#include "FormatOutput.h"

std::shared_ptr<spdlog::logger> LoggerManager::file_logger = nullptr;


std::shared_ptr<spdlog::logger> LoggerManager::get_unique_logger()
{
    static std::atomic<int> counter{0};
    std::ostringstream oss;
    oss << "UNIQUE_" << counter++;
    auto logger = spdlog::stdout_color_mt(oss.str());
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    return logger;
}

void LoggerManager::initializeFile()
{
    file_logger = spdlog::basic_logger_mt("global_file_logger", "D:\\UniversityKeep\\ContextManagementSystem\\bin\\spdlog_output.txt",true);
}