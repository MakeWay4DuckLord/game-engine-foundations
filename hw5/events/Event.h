#pragma once
#include <nlohmann/json.hpp>

enum EventType { 
    COLLISION,
    DEATH,
    SPAWN,
    INPUT,
};

/**
 * Event is a generic class that represents a condition to which some system in the engine or game may wish to respond to.
 * Events store a void *data which can be used to store some information needed to properly handle the event,
 * The EventType is represented with an enumeration, care should be used stay consistent with the what data type is stored for each EventType
 */
class Event {
    public:
        EventType type;
        std::string serialization;
        Event(EventType type, void *data);
        Event(EventType type);
        Event(std::string serialization);
        nlohmann::json serialize();
        void *data;
        friend bool operator< (Event const& e1, Event const& e2);
        void setTimestamp(unsigned int time);
        unsigned int timestamp;
};