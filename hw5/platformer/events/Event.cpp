#include "Event.h"

Event::Event(EventType type, void *data) {
    this->type = type;
    this->data = data;
    this->timestamp = 0;
    this->serialization = this->serialize().dump();
}

Event::Event(EventType type) {
    this->type = type;
    this->timestamp = 0;
    this->serialization = this->serialize().dump();
}

Event::Event(std::string serialization) {
    this->serialization = serialization;
    nlohmann::json event_j = nlohmann::json::parse(serialization); 
    this->type = event_j["type"];
    this->timestamp = event_j["timestamp"];
}

nlohmann::json Event::serialize() {
    nlohmann::json event_j;
    event_j["type"] = this->type;
    event_j["timestamp"] = this->timestamp;
    //sumn with the void*
    return event_j;
}


bool operator< (Event const& e1, Event const& e2) {
    //reverse < to > so events can be stored in a priority_queue as a min heap
    return e1.timestamp > e2.timestamp;
}

void Event::setTimestamp(unsigned int time) {
    timestamp = time;
}