#include "EventManager.h"

EventManager *EventManager::manager = getInstance();

EventManager::EventManager() {
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
    // EventManager *instance = EventManager::getInstance();
    for(EventHandler *handler : registered_handlers[e.type]) {
        handler->onEvent(e);
    }
}

void EventManager::registerHandler(std::vector<EventType> types, EventHandler *handler) {
    // EventManager *instance = EventManager::getInstance();
    for(EventType type : types) {
        registered_handlers[type].push_back(handler);
    }
}