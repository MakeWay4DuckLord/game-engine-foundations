#include "RenderComponent.h"
#include "game-objects/GameObject.h"

RenderComponent::RenderComponent(sf::RenderWindow *window) : ObjectComponent(NULL) {
    this->window = window;
}

void RenderComponent::draw() {
    window->draw(*object->getShape());
}