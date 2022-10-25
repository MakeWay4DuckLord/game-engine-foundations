#include "GameObject.h"
#include "components/MovementComponent.h"
#include "components/CollisionComponent.h"
#include "components/RenderComponent.h"


GameObject::GameObject(sf::Shape *shape, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent) {
    this->shape = shape;
    this->movementComponent = movementComponent;
    if(this->movementComponent) {
        this->movementComponent->setObject(this);
    }

    this->collisionComponent = collisionComponent;
    if(this->collisionComponent){
        this->collisionComponent->setObject(this);
    }

    this->renderComponent = renderComponent;
    if(this->renderComponent) {
        this->renderComponent->setObject(this);
    }
}

GameObject::GameObject(sf::Shape *shape) : GameObject(shape, NULL, NULL, NULL) {
    
}

sf::Shape *GameObject::getShape() {
    return shape;
}

void GameObject::update(unsigned int delta) {
    if(movementComponent) {
        this->movementComponent->updateMovement(delta);
    }
}