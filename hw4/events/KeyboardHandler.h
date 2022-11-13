#pragma once
#include "EventHandler.h"
#include "events/KeyboardEvent.h"

class KeyboardHandler : public EventHandler {
    public:
        KeyboardHandler();
        void onEvent(Event e);
};