#pragma once
#include "GameObject.h"

class Character : public GameObject {
    public:
        Character(sf::Shape *shape, float speed, sf::RenderWindow *window, GameObject *spawnPoint);
        void respawn();
        void setSpawnPoint(GameObject *spawnPoint);
    private:
        GameObject *spawnPoint;

};