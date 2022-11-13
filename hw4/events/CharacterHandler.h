#pragma once
#include "game-objects/Character.h"
#include "events/EventHandler.h"

class CharacterHandler : public EventHandler {
    public:
        CharacterHandler(Character *Character);
        void onEvent(Event e);
        
    private:
        Character *character;
};