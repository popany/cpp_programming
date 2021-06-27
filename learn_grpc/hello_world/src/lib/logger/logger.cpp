#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

void InitLogger(std::string logPath)
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("./hello_world.log", 0, 0));
	auto combined_logger = std::make_shared<spdlog::logger>("applog", begin(sinks), end(sinks));
	combined_logger->set_level(spdlog::level::info);
	combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][pid %P][tid %t][%s:%#][%!] %v");
	spdlog::set_default_logger(combined_logger);
}

void SetLogLevel(LogLevel logLevel)
{
    switch (logLevel) {
        case LogLevel::DEBUG:
            spdlog::set_level(spdlog::level::debug);
            break;
        case LogLevel::INFO:
            spdlog::set_level(spdlog::level::info);
            break;
        case LogLevel::WARN:
            spdlog::set_level(spdlog::level::warn);
            break;
        case LogLevel::ERROR:
            spdlog::set_level(spdlog::level::err);
            break;
        default:
            spdlog::set_level(spdlog::level::info);
    }
}
