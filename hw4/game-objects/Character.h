#pragma once
#include "game-objects/GameObject.h"
#include "game-objects/components/CharMoveComponent.h"

// class GameObject;

class Character : public GameObject {
    public:
        Character(std::string id, sf::Shape *shape, float speed, sf::RenderWindow *window, GameObject *spawnPoint);
        void respawn();
        void setSpawnPoint(GameObject *spawnPoint);
    private:
        GameObject *spawnPoint;
        CharMoveComponent *charMoveComp;

};