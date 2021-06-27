#pragma once

#include "event.h"

class EventHandler
{
public:
    virtual void process(bool optOk, Event event) = 0; 
    virtual bool isComplete() = 0;
};
