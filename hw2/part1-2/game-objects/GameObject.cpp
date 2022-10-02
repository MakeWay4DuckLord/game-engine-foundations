#include "GameObject.h"


GameObject::GameObject(sf::Vector2f size, sf::Vector2f position) : RectangleShape(size)
{
    setPosition(position);
}

// void GameObject::updateGameObject(unsinged int delta);