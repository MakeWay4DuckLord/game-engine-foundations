#include "Event.h"

Event::Event(EventType type, void *data) {
    this->type = type;
    this->data = data;
}

Event::Event(EventType type) {
    this->type = type;
}