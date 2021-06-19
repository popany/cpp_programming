#include "client_config.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "logger.h"

static const std::string CLIENT_CONFIG_FILE_NAME = "client.config";
static const char COMMENT_PREFIX = '#';

ClientConfig::ClientConfig()
{
    initConfig();
}

ClientConfig& ClientConfig::getInstance()
{
    static ClientConfig clientConfig;
    static int initConfig = (clientConfig.loadConfigFile(), 1);
    return clientConfig;
};

void ClientConfig::loadConfigFile()
{
    try {
        std::fstream s(CLIENT_CONFIG_FILE_NAME, std::fstream::in);
        if (!s.is_open()) {
            LOG_DEBUG("failed to open client config file \"{}\"", CLIENT_CONFIG_FILE_NAME);
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
