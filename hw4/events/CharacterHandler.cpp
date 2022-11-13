#include "events/CharacterHandler.h"
#include "events/EventManager.h"
#include "game-objects/components/CollisionComponent.h"

CharacterHandler::CharacterHandler(Character *character) {
    this->character = character;
}

void CharacterHandler::onEvent(Event e) {
    switch (e.type)
    {

    case EventType::COLLISION:
        if(true) {
        CollisionComponent *other = (CollisionComponent *) e.data;
        //do collision behavior
        if(other->isSolid()) {
            
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
        // object->move(sf::Vector2f(0.f,-10.f));
        break;
    
    case EventType::DEATH:
        EventManager::getInstance()->raise(*(new Event(EventType::SPAWN)));
        break;
    
    case EventType::SPAWN:
        this->character->respawn();
        break;

    default:
        break;
    }
}