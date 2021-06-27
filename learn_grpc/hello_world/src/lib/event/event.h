#pragma once
#include <stdint.h>

typedef uint32_t event_key_t;
typedef uint32_t event_opt_t;

class Event
{
    union {
        void* token;
        struct {
            event_key_t key;
            event_opt_t opt;
        };
    };

public:
    explicit Event(void* p) :
        token(p)
    {}

    explicit Event(event_key_t key) :
        key(key)
    {}

    void* getToken()
    {
        return token;
    }

    event_key_t getKey()
    {
        return key;
    }

    event_opt_t getOpt()
    {
        return opt;
    }

    void setOpt(event_opt_t opt)
    {
        this->opt = opt;
    }
};

static_assert(sizeof(Event) <= sizeof(void*));
