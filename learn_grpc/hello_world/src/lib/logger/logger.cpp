#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

void InitLogger()
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("../logs/hello_world.log", 0, 0));
	auto combined_logger = std::make_shared<spdlog::logger>("applog", begin(sinks), end(sinks));
	combined_logger->set_level(spdlog::level::info);
	combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][pid %P][tid %t][%s:%#][%!] %v");
	spdlog::set_default_logger(combined_logger);
}

void SetLogLevel(LOG_LEVEL logLevel)
{
    switch (logLevel) {
        case DEBUG:
            spdlog::set_level(spdlog::level::debug);
            break;
        case INFO:
            spdlog::set_level(spdlog::level::info);
            break;
        case WARN:
            spdlog::set_level(spdlog::level::warn);
            break;
        case ERROR:
            spdlog::set_level(spdlog::level::error);
            break;
        default:
            spdlog::set_level(spdlog::level::info);
    }
}
