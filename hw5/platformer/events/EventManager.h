#pragma once
// #include "Event.h"
#include "events/EventHandler.h"
#include "timeline/Timeline.h"
#include <map>
#include <vector>
#include <queue>
#include <v8.h>
#include "scripting/v8helpers.h"

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

        void raise(std::string serialization);

        void setTimeline(Timeline *basis);

        void handleEvents();


        /**
         * Callback method which that allows scripts to scripts to raise events
         */
        static void raiseEventCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
};

