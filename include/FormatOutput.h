#include <spdlog/spdlog.h>
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <atomic>
#include <sstream>

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
    static std::shared_ptr<spdlog::logger> get_unique_logger()
    {
        static std::atomic<int> counter{0};
        std::ostringstream oss;
        oss << "UNIQUE_" << counter++;
        auto logger = spdlog::stdout_color_mt(oss.str());
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        return logger;
    }
};