#include "MovementComponent.h"
#include "game-objects/GameObject.h"

MovementComponent::MovementComponent() : ObjectComponent(NULL) {
    //do nothing, no state
}

void MovementComponent::updateMovement(unsigned int delta) {
    object->getShape()->move(getVelocity(delta));
}