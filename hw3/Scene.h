#pragma once
// #include "game-objects/components/CollisionComponent.h"
// #include "game-objects/components/RenderComponent.h"
// #include "game-objects/Character.h" // forward declares a lot of things which will need to be included in source file
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

//gotta love forward declarations!

class Character;
class GameObject;
class CollisionComponent;
class RenderComponent;
class MovementComponent;


using json = nlohmann::json;

/**
 * Scene object stores necessary information for drawing a scene, 
 * including managing multiple views for a window,
 * and provides methods for generating and interpreting json objects,
 * which can be used to send updates between clients and server.
 */

class Scene
{
    public:



        //creates empty scene with given window
        Scene(sf::RenderWindow *window);

        //adds given object to appropriate map, and adds components to appropriate lists for the object type
        void addObject(GameObject *object);

        GameObject *createObject(std::string id, std::string type, sf::Vector2f size, sf::Vector2f position, sf::Color color, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent);

        //updates all objects
        void update(unsigned int delta);

        //updates environmental objects (everything but characters)
        void updateObjects(unsigned int delta);

        //updates character
        void updateCharacter(unsigned int delta);

        //checks for collisions between the character and other objects
        void calculateCollisions();

        //draws background and then foreground
        void draw();

        json serialize();

        void updateObjects(json update);

        void updateCharacter(json update);

        json getObjectUpdates();

        json getCharacterUpdate();

        void removeCharacter(std::string id);
        
    private:
        //map of integer ids to character pointers
        std::map<std::string, GameObject *> characters;

        //map of integer ids to GameObjects
        std::map<std::string, GameObject *> objects;

        //list of solid collision components that character can collide with
        std::vector<CollisionComponent*> terrain;

        //list of collision components which cause a character to respawn
        std::vector<CollisionComponent*> hazards;

        //list of RenderComponents which should be drawn last
        std::map<std::string ,RenderComponent*> foreground;

        //list of RenderComponents to be drawn first
        std::map<std::string, RenderComponent*> background;

        //viewBoundary object to keep the character in view
        GameObject *viewBoundary;

        GameObject *spawnPoint;

        //character which the client(or server) with this scene instance controls.
        Character *character;

        sf::RenderWindow *window;

        void scrollView();

        bool scrolling; 

        

        // void initializeUpdateLog();
        // json updateLog;
};