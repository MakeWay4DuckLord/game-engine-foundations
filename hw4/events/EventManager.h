#pragma once
// #include "Event.h"
#include "EventHandler.h"
// #include "timeline/Timeline.h"
#include <map>
#include <vector>

class EventManager {
    private:
        std::map<EventType, std::vector<EventHandler*>> registered_handlers;
        EventManager();
        static EventManager* manager;

    public:
        static EventManager* getInstance();
        // static void destroyInstance();
        void registerHandler(std::vector<EventType> types, EventHandler *handler);

        void raise(Event e);

        // static void handleEvents();
};

