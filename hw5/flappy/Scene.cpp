#include "Scene.h"
#include "game-objects/GameObject.h"
#include "game-objects/Character.h"
#include "game-objects/components/CollisionComponent.h"
#include "game-objects/components/RenderComponent.h"
#include "events/EventManager.h"
// #include "events/CharacterHandler.h"
#include <cmath>
#include <string>
#include <sstream>

static int score = 0;

static bool gameOver = false;

int Scene::getScore() {
    return score;
}

void Scene::incrementScore() {
    score++;
}

void Scene::lose(const v8::FunctionCallbackInfo<v8::Value> &args) {
    gameOver = true;
}


Scene::Scene(sf::RenderWindow *window) {
    font.loadFromFile("fonts/font.ttf");
    scoreboard.setFont(font);
    scoreboard.setCharacterSize(50);
    scoreboard.setFillColor(sf::Color::Red);
    scoreboard.setPosition(sf::Vector2f(20,0));
    this->window = window;
    this->time = 0;
    this->viewBoundary = NULL;
    this->spawnPoint = NULL;
    this->character = NULL;
    this->characterHandler = NULL;
    this->scrolling = false;
    this->characters = {};
    this->objects = {};
    this->collidables = {};
    this->foreground = {};
    this->background = {};
}

void Scene::setScriptManager(ScriptManager *sm) {
    this->sm = sm;
}

void Scene::addObject(GameObject *object) {
    if(object->getType() == TYPE_CHARACTER) {
        //first character added to a Scene is the one the client with that Scene controls
        if(characters.size() == 0) {
            character = (Character *) object;
            characterHandler = new CharacterHandler(character, sm);
            EventManager::getInstance()->registerHandler({EventType::DEATH, EventType::SPAWN, EventType::COLLISION, EventType::INPUT}, characterHandler);
        }
        //add character to characters map
        characters.emplace(object->getId(), object);
        if(object->renderComponent) {
            foreground.emplace(object->getId(), object->renderComponent);
        }
    } else {
        if(object->renderComponent) {
            background.emplace(object->getId(), object->renderComponent);
        }
        if(object->collisionComponent){
            collidables.push_back(object->collisionComponent);
            if(object->getType() == TYPE_VIEW_BOUNDARY) {
                viewBoundary = object;
            } else {
                objects.emplace(object->getId(), object);
            }
        } else {
            if(object->getType() == TYPE_SPAWN_POINT) {
                spawnPoint = object;
            }
        }
    }
}


GameObject *Scene::createObject(std::string id, std::string type, sf::Vector2f size, sf::Vector2f position, sf::Color color, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent) {
    sf::Shape *shape = new sf::RectangleShape(size);
    if(!type.compare(TYPE_VIEW_BOUNDARY)) {
        shape->setOrigin(size.x/2, size.y/2);
    }
    shape->setPosition(position);
    shape->setFillColor(color);

    GameObject *object;

    if(!type.compare(TYPE_CHARACTER)) {
        object = new Character(id, shape, 10, window, spawnPoint);
        addObject(object);
        return object;
        return NULL;
    }

    object = new GameObject(id, type, shape, movementComponent, collisionComponent, renderComponent);
    addObject(object);
    return object;
    
}

void Scene::updateObjects(unsigned int delta) {    
    for(const auto &o : objects) {
        o.second->update(delta);
    }
}

void Scene::updateCharacter(unsigned int delta) {
    if(window->hasFocus() && character != NULL) {
        //raise input events based on keyboard input
        characterHandler->pollInputs();
    }
    character->update(delta);
}

void Scene::calculateCollisions() {
    for(CollisionComponent *p : collidables) {
        character->collisionComponent->collision(p);
    }
    
    // if(viewBoundary && (!character->collisionComponent->collision(viewBoundary->collisionComponent) || scrolling)) {
    //     float x1 = viewBoundary->getShape()->getPosition().x;
    //     float x2 = character->getShape()->getPosition().x - character->getShape()->getGlobalBounds().width / 2;
    //     float y1 = viewBoundary->getShape()->getPosition().y;
    //     float y2 = character->getShape()->getPosition().y - character->getShape()->getGlobalBounds().height / 2;
    //     scrolling = true;
    //     float distance = sqrt(powf(x2 - x1, 2) + powf(y2 - y1, 2));
    //     if( distance > 1) {
    //         viewBoundary->getShape()->move(sf::Vector2f((x2 - x1) / distance, (y2 - y1) / distance));
    //     } else {
    //         scrolling = false;
    //     }
    //     scrollView();
    // }
}

bool Scene::update(unsigned int delta) {
    updateObjects(delta);
    updateCharacter(delta);
    calculateCollisions();
    EventManager::getInstance()->handleEvents();
    return gameOver;
}

void Scene::draw() {

    for(const auto &o : background) {
        o.second->draw();
    }

    for(const auto &o : foreground) {
        o.second->draw();
    }

    scoreboard.setString(std::to_string(Scene::getScore()));

    window->draw(scoreboard);
}

// void Scene::scrollView() {
//     sf::View currentView = window->getView();
//     currentView.setCenter(viewBoundary->getShape()->getPosition());
//     window->setView(currentView);
// }

json Scene::serialize() {
    std::vector<std::string> dumps;
    json serialization;
    for(const auto &o : objects) {
        //json object = o.second->serialize();
        dumps.push_back(o.second->serialize().dump());
    }

    for(const auto &c : characters) {
        // serialization[c.first] = c.second->serialize().dump();
        dumps.push_back(c.second->serialize().dump());
    }

    serialization["objects"] = dumps;
    serialization["clients"] = characters.size();
    return serialization;
}

json Scene::getCharacterUpdate() {
    json update;
    if(character == NULL) {
        update["iteration", 0];
    } else {
        update = character->serializeUpdate();
    }
    return update;
}

void Scene::updateCharacter(json update) {
    if(characters.count(update["id"]) == 0 ) {
        createObject(update["id"], TYPE_CHARACTER, sf::Vector2f(50, 50), sf::Vector2f(update["x"], update["y"]), sf::Color::Red, NULL, NULL, NULL);
    } else if(character->getId() != update["id"]){
        characters[update["id"]]->updateJson(update);
    }
}

json Scene::getObjectUpdates() {
    std::vector<std::string> dumps;
    json update;
    for(const auto &o : objects) {
        dumps.push_back(o.second->serializeUpdate().dump());
    }
    update["objects"] = dumps;
    return update;
}

void Scene::updateObjects(json update) {
    std::vector<std::string> dumps = update["objects"];//.get<std::vector<std::string>>();
    for(int i = 0; i < dumps.size(); i++) {
        json object_j = json::parse(dumps[i]);
        objects[object_j["id"]]->updateJson(object_j);
        // std::string dump = update[o.first.c_str()]; 
        // json objectUpdate = json::parse(dump.c_str());
        // o.second->update(objectUpdate);
    }
}

void Scene::removeCharacter(std::string id) {
    characters.erase(id);
    foreground.erase(id);
} 
