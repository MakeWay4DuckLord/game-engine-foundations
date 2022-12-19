#include "TrackingMoveComponent.h"
#include "game-objects/GameObject.h"

TrackingMoveComponent::TrackingMoveComponent(GameObject *target, sf::Vector2f offset) {
    this->target = target;
    this->offset = offset;
}

sf::Vector2f TrackingMoveComponent::getVelocity(unsigned int delta) {
    // sf::Vector2f newPosition = target.getShape()->getPosition();
    this->object->getShape()->setPosition(target->getShape()->getPosition() + offset);
    return {0,0};
}