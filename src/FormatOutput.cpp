#include "FormatOutput.h"
#include <sstream>

thread_local std::string thread_id_str;

std::shared_ptr<spdlog::logger> LoggerManager::file_logger = nullptr;
std::unordered_map<size_t, std::shared_ptr<spdlog::logger>> LoggerManager::regular_file_loggers;
std::unordered_map<std::thread::id, size_t> LoggerManager::thread_logger_map;
std::mutex LoggerManager::logger_mutex;
size_t LoggerManager::logger_count = 0;

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
    if (isNotRegular == true)
        return file_logger;
    else
        throw std::runtime_error("Invalid request for file logger.");
}

void LoggerManager::initializeFile()
{
    std::lock_guard<std::mutex> lock(logger_mutex);
    if (file_logger)
    {
        spdlog::drop(file_logger->name());
    }
    file_logger = spdlog::basic_logger_mt("global_file_logger", "..\\bin\\global_scenario_log.txt", true);
    file_logger->flush_on(spdlog::level::info);
}

void LoggerManager::initializeRegularFiles(size_t count)
{
    std::lock_guard<std::mutex> lock(logger_mutex);
    regular_file_loggers.clear();
    thread_logger_map.clear();
    logger_count = 0;
}

std::shared_ptr<spdlog::logger>& LoggerManager::getThreadFileLogger(bool notThread)
{
    if (notThread == true)
    {
        return getFileLogger(true);
    }

    std::lock_guard<std::mutex> lock(logger_mutex);
    auto thread_id = std::this_thread::get_id();

    if (thread_logger_map.find(thread_id) == thread_logger_map.end())
    {
        size_t new_index = logger_count++;
        std::ostringstream oss;
        oss << "regular_file_logger_" << new_index;
        std::string logger_name = oss.str();
        std::string file_name = "..\\bin\\threads-log\\thread_scenario_log_" + std::to_string(new_index + 1) + ".txt";

        auto logger = spdlog::basic_logger_mt(logger_name, file_name, true);
        logger->flush_on(spdlog::level::info);
        regular_file_loggers[new_index] = logger;
        thread_logger_map[thread_id] = new_index;
    }

    size_t index = thread_logger_map[thread_id];
    return regular_file_loggers[index];
}