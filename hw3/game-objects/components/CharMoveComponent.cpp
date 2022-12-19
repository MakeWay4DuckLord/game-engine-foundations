#include "CharMoveComponent.h"
#include "RenderComponent.h"
#include "game-objects/GameObject.h"

CharMoveComponent::CharMoveComponent(float speed) {
    this->speed = speed;
}

sf::Vector2f CharMoveComponent::getVelocity(unsigned int delta) {
    //by default you just fall
    sf::Vector2f velocity(0.f, speed * delta);

    //all characters should have a renderComponent, but check anyway, and only check for input if window has focus
    if(object->renderComponent && object->renderComponent->window->hasFocus()) {
        //"jump" is kinda just flight. physics based jump would be cool, but this works for now

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            velocity.y *= -1;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            velocity.x -= speed * delta;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            velocity.x += speed * delta;
        }
    }

    return velocity;
}