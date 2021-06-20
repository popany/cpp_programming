#include "abstract_config.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "logger.h"

static const char COMMENT_PREFIX = '#';

void AbstractConfig::init(const std::string& configFilePath)
{
    this->configFilePath = configFilePath;
    initConfig();
    loadConfigFile();
}

void AbstractConfig::loadConfigFile()
{
    try {
        std::fstream s(configFilePath, std::fstream::in);
        if (!s.is_open()) {
            LOG_WARN("failed to open client config file \"{}\"", configFilePath);
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
