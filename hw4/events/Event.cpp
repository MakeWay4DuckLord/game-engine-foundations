#include "Event.h"

Event::Event(EventType type, void *data) {
    this->type = type;
    this->data = data;
    this->timestamp = 0;
}

Event::Event(EventType type) {
    this->type = type;
    this->timestamp = 0;
}


bool operator< (Event const& e1, Event const& e2) {
    //reverse < to > so events can be stored in a priority_queue as a min heap
    return e1.timestamp > e2.timestamp;
}

void Event::setTimestamp(unsigned int time) {
    timestamp = time;
}