#pragma once
#include "game-objects/Character.h"
#include "events/EventHandler.h"
#include "scripting/ScriptManager.h"
#include <SFML/Graphics.hpp>


static sf::Keyboard::Key LEFT = sf::Keyboard::A;
static sf::Keyboard::Key RIGHT = sf::Keyboard::D;
static sf::Keyboard::Key JUMP = sf::Keyboard::W;

class CharacterHandler : public EventHandler {
    public:
        CharacterHandler(Character *Character, ScriptManager *sm);//, v8::Local<v8::ObjectTemplate> global);
        void onEvent(Event e);
        static void pollInputs();
    private:
        Character *character;
        ScriptManager *sm;
        v8::Local<v8::ObjectTemplate> global;
};