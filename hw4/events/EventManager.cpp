#include "EventManager.h"

EventManager *EventManager::manager = getInstance();

EventManager::EventManager() {
    pQueue = std::priority_queue<Event>();
    registered_handlers = {};//new std::map<EventType, std::vector<EventHandler*> *>();
    registered_handlers.emplace(EventType::COLLISION, std::vector<EventHandler*>());
    registered_handlers.emplace(EventType::DEATH, std::vector<EventHandler*>());
    registered_handlers.emplace(EventType::INPUT, std::vector<EventHandler*>());
    registered_handlers.emplace(EventType::SPAWN, std::vector<EventHandler*>());
}

EventManager* EventManager::getInstance() {
    if(manager == nullptr) {
        manager = new EventManager();
    } 
    return manager;
}

void EventManager::raise(Event e) {
    //handle inputs immediately, so that character will know the input 
    if(e.type == EventType::INPUT) {
        for(EventHandler *handler : registered_handlers[EventType::INPUT]) {
            handler->onEvent(e);
        }
        return;
    } 

    e.setTimestamp(timeline->getTime());
    pQueue.push(e);
}

void EventManager::registerHandler(std::vector<EventType> types, EventHandler *handler) {
    for(EventType type : types) {
        registered_handlers[type].push_back(handler);
    }
}

void EventManager::setTimeline(Timeline *timeline) {
    this->timeline = timeline;
}

void EventManager::handleEvents() {
    while(!pQueue.empty()) {
        Event e = pQueue.top();
        for(EventHandler *handler : registered_handlers[e.type]) {
            handler->onEvent(e);
        }
        pQueue.pop();
    }
}