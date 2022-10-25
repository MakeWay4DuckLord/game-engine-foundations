#include "CollisionComponent.h"
#include "game-objects/GameObject.h"
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

        // if we overlap
    if(ourBounds.intersects(theirBounds)) {
        //do collision behavior
        if(other->isSolid()) {
            if(ourBounds.top + ourBounds.height / 3 < theirBounds.top) {
                object->getShape()->setPosition(ourBounds.left,theirBounds.top - ourBounds.height);
            } else if(ourBounds.left + ourBounds.width > theirBounds.left) {
                object->getShape()->setPosition(theirBounds.left + theirBounds.width, ourBounds.top);
            } else {
                object->getShape()->setPosition(ourBounds.left, theirBounds.top + theirBounds.height);
            }
        }
        // object->move(sf::Vector2f(0.f,-10.f));
        return true;
    }
    return false;
}