#pragma once
#include "game-objects/Platform.h"
#include "game-objects/Character.h"
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

using json = nlohmann::json;

/**
 * A GameState object has vectors of game objects, methods to update them using their updateGameObject methods, 
 * as well as to update them given a json object of the correct format. Additionally a GameState object can be constructed using a json.
 */

class GameState
{
    public:
        //map of integer ids to character pointers
        std::map<int, Character *> characters;
        //map of integer ids to platform pointers (including moving platforms)
        std::map<int, Platform *> platforms;
        //default constructor creates empty maps
        GameState();
        //contruct GameState from json produced by GameState::serialize, notably doesn't support MovingPlatforms, so only suitable for clients that will only update platform position via instructions from server rather than the updateGameObject method
        GameState(json data);
        /*
        converts GameState object into a json object
        */
        json serialize();
        //returns the total number of characters and platforms
        int count();
        //adds give int character pair to the map
        void addCharacter(int id, Character *character);
        //adds anew character from the given json representation
        void addCharacter(json character_j);
        //adds given int platform pair to the map
        void addPlatform(int id, Platform *platform);
        //draws all characters to given window
        void drawCharacters(sf::RenderWindow *window);
        //draws all platforms to given window
        void drawPlatforms(sf::RenderWindow *window);
        //draws all platforms and characters to given window
        void drawAll(sf::RenderWindow *window);
        //calls updateGameObject(delta) on character specified by id 
        void updateCharacter(int id, unsigned int delta);
        //calls updateGameObjects(delta) on all platforms
        void updatePlatforms(unsigned int delta);
        /*
        updates position of a character using the given json object,
        which must be of the form:
        {
            {"clientID": int}
            {"position.x": float}
            {"position.y": float}
        }
        */

        void updateCharacter(json update);
        /*
        updates position of a platform using the given json object,
        which must be of the form:
        {
            {"platformID": int}
            {"position.x": float}
            {"position.y": float}
        }
        */
        void updatePlatform(json update);

        //returns a json object containing all the updates that have happed to objects since log was cleared
        json getUpdateLog();
        //returns the resets the json object containing updates since last time this method was called
        json clearUpdateLog();

        //updates state according to given json object
        void update(json log);
        
    private:
        void initializeUpdateLog();
        json updateLog;
};