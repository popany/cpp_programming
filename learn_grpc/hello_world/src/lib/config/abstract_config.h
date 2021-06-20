#pragma once

#include <string>

#define DEFINE_CONFIG_ITEM(item,type,parser,default)\
    type _##item;\
    void init_##item()\
    {\
        _##item = parser(default);\
    }\
    bool set_##item(const std::string& k, const std::string& v)\
    {\
        if (k != item) {\
            return false;\
        }\
        _##item = parser(v);\
        return true;\
    }\
    public:\
    const type& GET_##item() const\
    {\
        return _##item;\
    }\
    private:

#define INIT_CONFIG(item) do {\
        init_##item();\
    } while (0)

#define SET_CONFIG(item,k,v) set_##item(k, v)

class AbstractConfig
{
    std::string configFilePath;
protected:
    virtual void initConfig() = 0;
    virtual void setConfig(const std::string& name, const std::string& value) = 0;
    void loadConfigFile();
    void init(const std::string& configFilePath);
};
