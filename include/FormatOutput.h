#include <iostream>
#include <memory>
#include <vector>
#include <atomic>
#include <sstream>
#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

class LoggerManager
{
public:
    static std::shared_ptr<spdlog::logger> get_unique_logger();
    static void initializeFile();
    static std::shared_ptr<spdlog::logger>& getFileLogger() { return file_logger; }

private:
    static std::shared_ptr<spdlog::logger> file_logger;
};
