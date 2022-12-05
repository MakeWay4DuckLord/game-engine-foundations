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
        if(other->isSolid()) {
        // this->sm->runOne("hello_world");
            
            sf::FloatRect ourBounds = character->getShape()->getGlobalBounds();
            sf::FloatRect theirBounds = other->getBounds();

            //colliding from top
            if(ourBounds.top + ourBounds.height / 3 < theirBounds.top) {
                character->getShape()->setPosition(ourBounds.left,theirBounds.top - ourBounds.height);
            //colliding from right
            } else if(ourBounds.left < theirBounds.left + theirBounds.width && ourBounds.left + ourBounds.width > theirBounds.left + theirBounds.width) {
                character->getShape()->setPosition(theirBounds.left + theirBounds.width, ourBounds.top);
            //colliding from left    
            } else if(ourBounds.left < theirBounds.left ){
                character->getShape()->setPosition(theirBounds.left - ourBounds.width, ourBounds.top);
            } else {
                character->getShape()->setPosition(ourBounds.left, theirBounds.top + theirBounds.height);
            }
        }

        if(other->object->getType() == TYPE_DEATH_ZONE) {
            EventManager::getInstance()->raise(*new Event(EventType::DEATH));
        }
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
    
    if(sf::Keyboard::isKeyPressed(JUMP)) {
        EventManager::getInstance()->raise(*(new Event(EventType::INPUT, &JUMP)));
    }

}