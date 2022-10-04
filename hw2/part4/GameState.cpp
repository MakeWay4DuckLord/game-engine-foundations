#include "GameState.h"
#include "game-objects/Platform.h"
#include "game-objects/Character.h"
#include <iostream>


using json = nlohmann::json;

int fail(std::string msg) {
    std::cerr << msg << std::endl;
    return EXIT_FAILURE;
}

GameState::GameState() {
    this->platforms = {};
    this->characters = {};
    // GameState({}, {});
    initializeUpdateLog();
}


GameState::GameState(json data) {
    // std::vector<std::vector<std::vector<float>>>  = data["platforms"];
    std::map<int, std::vector<float>> vCharacters = data["characters"];
    std::map<int, std::vector<float>> vPlatforms = data["platforms"];

    std::map<int, Character*> characters;
    std::map<int,Platform*> platforms;

    for(const auto &c : vCharacters) {
        Character *character = new Character(sf::Vector2f(c.second[0], c.second[1]), sf::Vector2f(c.second[2], c.second[3]));
        characters.insert(std::pair<int, Character*>(c.first, character));
    }

    // characters = {};


    for(const auto &p : vPlatforms) {
        Platform *platform = new Platform(sf::Vector2f(p.second[0], p.second[1]), sf::Vector2f(p.second[2], p.second[3]));
        platforms.insert(std::pair<int, Platform*>(p.first, platform));
    }

    this->platforms = platforms;
    this->characters = characters;

    
}

void GameState::addCharacter(int id, Character *character) {
    if(characters.count(id) == 0) {
        this->characters.insert(std::pair<int, Character*>(id, character));
        json newChar = {
            {"clientID", id},
            {"position.x", character->getPosition().x},
            {"position.y", character->getPosition().y},
            {"size.x", character->getSize().x},
            {"size.y", character->getSize().y},
        };

        this->updateLog["new characters"].push_back(newChar);
    }
}

void GameState::addCharacter(json character_j) {
    int id = character_j["clientID"];
    if(characters.count(id) == 0) {
        sf::Vector2f position(character_j["position.x"], character_j["position.y"]);
        sf::Vector2f size(character_j["size.x"], character_j["size.y"]);
        this->characters.emplace(id, new Character(size, position));
    }
}

void GameState::addPlatform(int id, Platform *platform) {
    this->platforms.emplace(id, platform);
}

int GameState::count(){
    return platforms.size() + characters.size();
}

json GameState::serialize() {
    std::map<int, std::vector<float>> vPlatforms;
    std::map<int, std::vector<float>> vCharacters;

    for(const auto &c : characters) {
        std::vector<float> vCharacter{c.second->getSize().x, c.second->getSize().y, c.second->getPosition().x, c.second->getPosition().y};
        vCharacters.insert(std::pair<int, std::vector<float>>(c.first, vCharacter));
    }

    for(const auto &p : platforms) {
        std::vector<float> vPlatform{p.second->getSize().x, p.second->getSize().y, p.second->getPosition().x, p.second->getPosition().y};
        vPlatforms.insert(std::pair<int, std::vector<float>>(p.first, vPlatform));
    }

    json jData{
        {"characters", vCharacters},
        {"platforms", vPlatforms},
    };
    return jData;
}

void GameState::drawCharacters(sf::RenderWindow *window) {
    //TODO thread safe stuff
    for(const auto &c : characters) {
        window->draw(*c.second);
    }
}

void GameState::drawAll(sf::RenderWindow *window) {
    drawCharacters(window);
    drawPlatforms(window);
}

void GameState::drawPlatforms(sf::RenderWindow *window) {
    //TODO thread safe stuff
    for(const auto &p : platforms) {
        window->draw(*p.second);
    }
}

void GameState::updateCharacter(int id, unsigned int delta) {
    Character *character = characters[id];
    character->updateGameObject(delta);
    json update_j = {
        {"clientID", id},
        {"position.x", character->getPosition().x},
        {"position.y", character->getPosition().y},
    };

    updateLog["character updates"].push_back(update_j);
}

void GameState::updateCharacter(json update) {
    if(characters.count(update["clientID"]) == 1) {
    characters[update["clientID"]]->setPosition(sf::Vector2f(update["position.x"], update["position.y"]));
    updateLog["character updates"].push_back(update);
    }
}

void GameState::updatePlatforms(unsigned int delta) {
    for(const auto &p : platforms) {
        sf::Vector2f pos = p.second->getPosition();
        p.second->updateGameObject(delta);
        if(pos.x != p.second->getPosition().x || pos.y != p.second->getPosition().y) {
            pos = p.second->getPosition();
            json update_j = {
                {"platformID", p.first},
                {"position.x", pos.x},
                {"position.y", pos.y},
            };
            this->updateLog["platform updates"].push_back(update_j);
        }
    }
}

void GameState::updatePlatform(json update) {
    platforms[update["platformID"]]->setPosition(sf::Vector2f(update["position.x"], update["position.y"]));
}

void GameState::initializeUpdateLog() {
    this->updateLog = {
        {"platform updates", {}},
        {"character updates", {}},
        {"new characters", {}},
    };
}

json GameState::getUpdateLog() {
    return this->updateLog;
}

json GameState::clearUpdateLog() {
    json output = this->updateLog;
    initializeUpdateLog();
    this->updateLog["new characters"] = output["new characters"];
    return output;
}

void GameState::update(json update) {
    for(json newChar : update["new characters"]) {
        addCharacter(newChar);
    }

    for(json pUpdate : update["platform updates"]) {
        updatePlatform(pUpdate);
    }

    // if(update["character updates"] != NULL) {
        for(json cUpdate : update["character updates"]) {
            updateCharacter(cUpdate);
        }
    // }
}