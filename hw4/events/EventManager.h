#pragma once
// #include "Event.h"
#include "events/EventHandler.h"
#include "timeline/Timeline.h"
#include <map>
#include <vector>
#include <queue>

class EventManager {
    private:
        std::map<EventType, std::vector<EventHandler*>> registered_handlers;
        std::priority_queue<Event> pQueue;
        EventManager();
        static EventManager* manager;
        Timeline* timeline;
        

    public:
        static EventManager* getInstance();
        
        void registerHandler(std::vector<EventType> types, EventHandler *handler);

        void raise(Event e);

        void setTimeline(Timeline *basis);

        void handleEvents();
};

