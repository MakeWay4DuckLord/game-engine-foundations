#pragma once
#include "game-objects/components/ObjectComponent.h" //object component forward declares GameObject
// #include "game-objects/GameObject.h"
#include <SFML/Graphics/Shape.hpp>
#include <string.h>
#include <v8.h>

class CollisionComponent : public ObjectComponent
{
    public:
        CollisionComponent(bool scripted, std::string *scriptHandle = NULL);
        void updateComponent();
        /**
         * checks for collision with given CollisionComponent
         * updates this components containing object accordingly
         */
        bool collision(CollisionComponent *other);
        sf::FloatRect getBounds();
        bool hasScript();
        void setOnCollisionScript(std::string handle, std::string path=NULL);
        // void createContext()

    private:
        bool scripted;
        std::string scriptHandle;

};