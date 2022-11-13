#pragma once

enum EventType { 
    COLLISION,
    DEATH,
    SPAWN,
    INPUT
};

/**
 * Event is a generic class that represents a condition to which some system in the engine or game may wish to respond to.
 * Event types are represented by an enumeration, and derived classes may provide additional data needed to properly handle the event.
 */
class Event {
    public:
        EventType type;
        Event(EventType type, void *data);
        Event(EventType type);
        void *data;

        // bool replicated;
};