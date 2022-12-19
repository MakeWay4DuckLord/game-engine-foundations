#include "events/CharacterHandler.h"
#include "events/EventManager.h"
#include "game-objects/components/CollisionComponent.h"
#include "game-objects/components/CharMoveComponent.h"
#include <iostream>


CharacterHandler::CharacterHandler(Character *character, ScriptManager *sm) {
    this->character = character;
    this->sm = sm;
}

void CharacterHandler::onEvent(Event e) {
    // return;
    switch (e.type)
    {
    case EventType::COLLISION:
        if(true) {
        CollisionComponent *other = (CollisionComponent *) e.data;

        //do collision behavior
        v8::Local<v8::Context> collision_context = sm->getContextContainer("collision_context").context;
        other->object->exposeToV8("obstacle", sm->getContextContainer("collision_context").isolate, collision_context, "collision_context");
        // ((GameObject*)this->character)->exposeToV8(sm->getContextContainer("collision_context").isolate, collision_context, "collision_context");
        if(other->hasScript()) {
        // this->sm->runOne("hello_world");
            sm->runOne(*((std::string *)other->data), true, "collision_context"); 
           
        }

        // if(other->object->getType() == TYPE_DEATH_ZONE) {
        //     EventManager::getInstance()->raise(*new Event(EventType::DEATH));
        // }
        }
        break;
    
    case EventType::DEATH:
        EventManager::getInstance()->raise(*(new Event(EventType::SPAWN)));
        break;
    
    case EventType::SPAWN:
        this->character->respawn();
        break;

    case EventType::INPUT:

        if(true) {    
            sf::Keyboard::Key *key = (sf::Keyboard::Key *) e.data;             
            CharMoveComponent *movementComp = (CharMoveComponent *) character->movementComponent;
            movementComp->enqueueInput(*key);
        }
        break;
    default:
        break;
    }
}

void CharacterHandler::pollInputs() {

    if(sf::Keyboard::isKeyPressed(LEFT)) {
        EventManager::getInstance()->raise(*(new Event(EventType::INPUT, &LEFT)));
    }

    if(sf::Keyboard::isKeyPressed(RIGHT)) {
        EventManager::getInstance()->raise(*(new Event(EventType::INPUT, &RIGHT)));
    }

     
    if(sf::Keyboard::isKeyPressed(JUMP) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        EventManager::getInstance()->raise(Event(EventType::DEATH));

    } else if(sf::Keyboard::isKeyPressed(JUMP)) {
        EventManager::getInstance()->raise(*(new Event(EventType::INPUT, &JUMP)));
    }

}