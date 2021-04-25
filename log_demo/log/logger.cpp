#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

void InitLogger()
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/demo.log", 0, 0));
	auto combined_logger = std::make_shared<spdlog::logger>("applog", begin(sinks), end(sinks));
	combined_logger->set_level(spdlog::level::debug);
	combined_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][pid %P][tid %t][%s][%!] %v");  // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
	spdlog::set_default_logger(combined_logger);
}

void SetLogLevel(std::string level)
{
	if (level == "debug") {
		spdlog::set_level(spdlog::level::debug);
	}
	else if (level == "info") {
		spdlog::set_level(spdlog::level::info);
	}
	else if (level == "warn") {
		spdlog::set_level(spdlog::level::warn);
	}
	else if (level == "error") {
		spdlog::set_level(spdlog::level::err);
	}
}
