#pragma once
#include "game-objects/components/ObjectComponent.h"
#include <SFML/Graphics.hpp>

class RenderComponent : public ObjectComponent {
    public:
        RenderComponent(sf::RenderWindow *window);
        void draw();
    private:
        sf::RenderWindow *window;
};