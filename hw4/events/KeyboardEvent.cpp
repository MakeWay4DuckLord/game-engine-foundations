#include "events/KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(sf::Keyboard::Key k) : Event(EventType::INPUT) {
    key = k;
    data = &key;
}