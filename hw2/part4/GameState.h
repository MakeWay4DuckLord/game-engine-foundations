#pragma once
#include "game-objects/GameObject.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * A GameState object has vectors of game objects, methods to update them using their updateGameObject methods, 
 * as well as to update them given a json object of the correct format. Additionally a GameState object can be constructed using a json.
 */

class GameState
{
    public:
        std::vector<GameObject *> characters;
        std::vector<GameObject *> platforms;
        GameState();
        // GameState(std::vector<GameObject *> characters, std::vector<GameObject *> platforms);
        GameState(json data);
        // bool update(json data);
        json serialize();
        int count();
};