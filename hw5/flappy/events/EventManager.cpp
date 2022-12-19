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
    e.setTimestamp(timeline->getTime());
    if(e.type == EventType::INPUT) {
        for(EventHandler *handler : registered_handlers[EventType::INPUT]) {
            handler->onEvent(e);
        }
        return;
    } 

    pQueue.push(e);
}

void EventManager::raise(std::string serialization) {
    this->raise(Event(serialization));
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


//scriptable stuff:

/**
 * raises event
 */
void EventManager::raiseEventCallback(const v8::FunctionCallbackInfo<v8::Value>& args){
    if(args.Length() != 1) {
        printf("attempted to raise too much shit");
        return;
    }

    v8::String::Utf8Value str(args.GetIsolate(), args[0]);
    const char *dump = v8helpers::ToCString(str);
    printf("%s\n", dump);
    EventManager::getInstance()->raise(std::string(dump));
}