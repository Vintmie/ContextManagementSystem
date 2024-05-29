#include <iostream>
#include <memory>
#include <vector>
#include <atomic>
#include <sstream>
#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <unordered_map>
#include <mutex>
#include <thread>

enum class ResultType;

template <>
struct fmt::formatter<ResultType>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ResultType& resultType, FormatContext& ctx)
    {
        switch (resultType)
        {
            case ResultType::SUCCESS: return fmt::format_to(ctx.out(), "SUCCESS");
            case ResultType::FAILURE: return fmt::format_to(ctx.out(), "FAILURE");
            case ResultType::UNKNOWN: return fmt::format_to(ctx.out(), "UNKNOWN");
        }
        return fmt::format_to(ctx.out(), "INVALID_RESULT_TYPE");
    }
};

enum class ExecutionTypeCondition;

template <>
struct fmt::formatter<ExecutionTypeCondition>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ExecutionTypeCondition& condition, FormatContext& ctx)
    {
        switch (condition)
        {
            case ExecutionTypeCondition::SUCCESS: return fmt::format_to(ctx.out(), "SUCCESS");
            case ExecutionTypeCondition::FAILURE: return fmt::format_to(ctx.out(), "FAILURE");
            case ExecutionTypeCondition::UNCONDITIONAL: return fmt::format_to(ctx.out(), "UNCONDITIONAL");
        }
        // Return a default message in case none of the cases matched
        return fmt::format_to(ctx.out(), "INVALID_CONDITION_TYPE");
    }
};


extern thread_local std::string thread_id_str;


class LoggerManager
{
public:
    static std::shared_ptr<spdlog::logger> get_unique_logger();
    static void initializeFile();
    static void initializeRegularFiles(size_t count);
    static std::shared_ptr<spdlog::logger>& getFileLogger(bool isNotRegular = true);
    static std::shared_ptr<spdlog::logger>& getThreadFileLogger(bool notThread = true);

private:
    static std::shared_ptr<spdlog::logger> file_logger;
    static std::unordered_map<size_t, std::shared_ptr<spdlog::logger>> regular_file_loggers;
    static std::unordered_map<std::thread::id, size_t> thread_logger_map;
    static std::mutex logger_mutex;
    static size_t logger_count;
};
