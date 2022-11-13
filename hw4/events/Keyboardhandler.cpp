#include "KeyboardHandler.h"
#include <SFML/Graphics.hpp>
#include <iostream>
void KeyboardHandler::onEvent(Event e) {
    // if(e.type != EventType::INPUT) {
    //     return;
    // }

    switch (e.type)
    {
    
    case EventType::INPUT:
        std::cout << "input" << std::endl;
        break;
    
    case EventType::DEATH:
        std::cout << "death" << std::endl;
        break;
    
    default:
        break;
    }
}

KeyboardHandler::KeyboardHandler() {
    //do no state as of yet
}