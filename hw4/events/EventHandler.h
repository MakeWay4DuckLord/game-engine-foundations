#pragma once
#include "Event.h"

//an event handler provides functionality to respond to events
class EventHandler {
    public:
        // EventHandler();
        virtual void onEvent(Event e) = 0;
};