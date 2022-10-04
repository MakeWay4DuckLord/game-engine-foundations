#include "GameState.h"
#include "game-objects/Platform.h"
#include "game-objects/Character.h"
#include <iostream>


using json = nlohmann::json;

int fail(std::string msg) {
    std::cerr << msg << std::endl;
    return EXIT_FAILURE;
}

// GameState::GameState(std::vector<GameObject *> characters, std::vector<GameObject *> platforms) {
//     this->platforms = platforms;
//     this->characters = characters;
// }

GameState::GameState() {
    this->platforms = {};
    this->characters = {};
    // GameState({}, {});
}


GameState::GameState(json data) {
    // std::vector<std::vector<std::vector<float>>>  = data["platforms"];
    std::vector<std::vector<float>> vCharacters = data.at(0);
    std::vector<std::vector<float>> vPlatforms = data.at(1);

    std::vector<GameObject *> characters;
    std::vector<GameObject *> platforms;

    for(std::vector<float> &c : vCharacters) {
        GameObject *character = new Character(sf::Vector2f(c[0], c[1]), sf::Vector2f(c[2], c[3]));
        characters.push_back(character);
    }
    
    for(std::vector<float> &p : vPlatforms) {
        GameObject *platform = new Platform(sf::Vector2f(p[0], p[1]), sf::Vector2f(p[2], p[3]));
        platforms.push_back(platform);
    }

    this->platforms = platforms;
    this->characters = characters;

    
}

int GameState::count(){
    return platforms.size() + characters.size();
}

json GameState::serialize() {
    std::vector<std::vector<float>> vPlatforms;
    std::vector<std::vector<float>> vCharacters;

    for(GameObject* &c : characters) {
        std::vector<float> vCharacter{c->getSize().x, c->getSize().y, c->getPosition().x, c->getPosition().y};
        vCharacters.push_back(vCharacter);
    }

    for(GameObject* &p : platforms) {
        std::vector<float> vPlatform{p->getSize().x, p->getSize().y, p->getPosition().x, p->getPosition().y};
        vPlatforms.push_back(vPlatform);
    }

    std::vector<std::vector<std::vector<float>>> vData{vCharacters, vPlatforms};

    json jData(vData);
    return jData;
}