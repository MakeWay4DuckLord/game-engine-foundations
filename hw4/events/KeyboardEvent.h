#pragma once
#include "events/Event.h"
#include <SFML/Graphics.hpp>

class KeyboardEvent : public Event {
    public: 
        KeyboardEvent(sf::Keyboard::Key key);
        sf::Keyboard::Key key;
};