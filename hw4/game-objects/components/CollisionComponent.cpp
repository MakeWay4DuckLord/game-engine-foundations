#include "CollisionComponent.h"
#include "game-objects/GameObject.h"
#include "events/EventManager.h"
CollisionComponent::CollisionComponent(bool solid) : ObjectComponent(NULL)
{
    this->solid = solid;
}

void CollisionComponent::updateComponent() {
    //do nothing, behavior is handled by the collision method
}

sf::FloatRect CollisionComponent::getBounds() {
    return object->getShape()->getGlobalBounds();
}

bool CollisionComponent::isSolid() {

    return this->solid;
}

bool CollisionComponent::collision(CollisionComponent *other) {
    // //local var for collision bounds
    if(other == NULL) {
        return false;
    }

        sf::FloatRect ourBounds = this->getBounds();
        sf::FloatRect theirBounds = other->getBounds();

        // if we overlap raise a collision event
    if(ourBounds.intersects(theirBounds)) {

        EventManager::getInstance()->raise( *(new Event(EventType::COLLISION, other)));
        
        return true;
    }
    return false;
}