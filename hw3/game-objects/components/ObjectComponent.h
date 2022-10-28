#pragma once
// #include <nlohmann/json.hpp>
// #include <string.h>
// #include "game-objects/GameObject.h"

//forward declaration
class GameObject;

// enum componentType {
//     COLLISION,
//     MOVEMENT,
//     RENDER
// };

// using json = nlohmann::json;
/**
 * ObjectComponent is a virtual class for components of game objects
 * All ObjectComponents will have some data, and some behavior,
 * as well as a serialize function which allows them to be easily converted to json
 */ 
class ObjectComponent 
{
    public:
        // componentType type;
        ObjectComponent(void *data);
        //pointer to the GameObject which contains this component
        GameObject *object;
        void *data;
        //sets the components pointer to the GameObject containing it
        void setObject(GameObject *object);
};