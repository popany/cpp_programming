#include "config.h"
#include <fstream>
#include <iostream>
#include "utils.h"

Config::Config():
    port(DEFAULT_PORT),
    epEventCount(EP_EVENT_COUNT)
{
    std::ifstream fs("./config");
    if (fs.fail()) {
        std::cout << "failed to open config file" << std::endl;
        exit(1);
    }
    
    while (!fs.eof()) {
        std::string s;
        std::getline(fs, s);
        ParseLine(s);
    }
}

void Config::ParseLine(std::string s)
{
    Trim(s);
    size_t pos = s.find('=');
    if (pos == std::string::npos) {
        return;
    }

    std::string name = s.substr(0, pos);
    Trim(name);
    std::string value = s.substr(pos + 1);
    Trim(value);
    
    if (name == "port") {
        port = std::stoi(value);
    } else if (name == "log_level") {
        logLevel = value;
    }
}

Config& Config::GetInstance()
{
    static Config config;
    return config;
}
