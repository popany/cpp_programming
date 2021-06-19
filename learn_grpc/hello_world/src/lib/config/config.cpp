#include "config.h"

#define CONFIG_FILE_NAME "hello_word.config"

Config::Config():
    logLevel(LOG_LEVEL::INFO)
{}

Config& Config::getInstance()
{
    static Config config;
    static int initConfig = (config.loadConfigFile()), 1);
    return config;
};

void Config::loadConfigFile()
{
    try {
        std::string fileName = CONFIG_FILE_NAME;

        std::fstream s(CONFIG_FILE_NAME, std::fstream::in);
        if (!s.is_open()) {
            LOG_DEBUG("failed to open config file \"{}\"", CONFIG_FILE_NAME);
            return;
        }

        for (std::string line; std::getline(s, line); ) {
            boost::algorithm::trim(line);
            if (line[0] == COMMENT_PREFIX || line.empty()) {
                continue;
            }

            std::vector<std::string> kv;
            boost::split(kv, line, boost::is_any_of("="));
            if (kv.size() != 2) {
                continue;
            }
            for (int i = 0; i < kv.size(); i++) {
                boost::algorithm::trim(kv[i]);
            }
            setConfig(kv[0], kv[1]);
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception: {}", e.what());
    }
}
