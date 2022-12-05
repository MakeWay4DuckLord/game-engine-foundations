#include "GameObject.h"
#include "components/MovementComponent.h"
#include "components/CollisionComponent.h"
#include "components/RenderComponent.h"
#include <SFML/Graphics/Shape.hpp>
// #include <SFML/Graphics.hpp>
#include <vector>


GameObject::GameObject(std::string id, std::string type, sf::Shape *shape, MovementComponent *movementComponent, CollisionComponent *collisionComponent, RenderComponent *renderComponent) {
    this->id = id;
    this->type = type;
    this->shape = shape;
    this->movementComponent = movementComponent;
    if(this->movementComponent) {
        this->movementComponent->setObject(this);
    }

    this->collisionComponent = collisionComponent;
    if(this->collisionComponent){
        this->collisionComponent->setObject(this);
    }

    this->renderComponent = renderComponent;
    if(this->renderComponent) {
        this->renderComponent->setObject(this);
    }
}

GameObject::GameObject(std::string id, std::string type, sf::Shape *shape) : GameObject(id, type, shape, NULL, NULL, NULL) {
    
}

sf::Shape *GameObject::getShape() {
    return shape;
}

void GameObject::update(unsigned int delta) {
    if(movementComponent) {
        this->movementComponent->updateMovement(delta);
    }
}

std::string GameObject::getType() {
    return type;
}

std::string GameObject::getId() {
    return id;
}

json GameObject::serialize() {
    bool drawn = (renderComponent != NULL);
    bool solid = (collisionComponent != NULL && collisionComponent->isSolid());
    sf::Color color = getShape()->getFillColor();
    float shapeArray[4];
    shapeArray[0] = getShape()->getGlobalBounds().height;
    shapeArray[1] = getShape()->getGlobalBounds().width;
    shapeArray[2] = getShape()->getPosition().x;
    shapeArray[3] = getShape()->getPosition().y;

    int colorArray[4];
    colorArray[0] = color.r;
    colorArray[1] = color.g;
    colorArray[2] = color.b;
    colorArray[3] = color.a;
    json serialization;
    //  = {
    //     {"id", getId()},
    //     {"type", getType()},
    //     {"drawn", drawn},
    //     {"color", {color.r, color.g, color.b, color.a}},
    //     {"solid", solid},
    //     {"shape", {getShape()->getGlobalBounds().height(), getShape()->getGlobalBounds().width(), getShape()->getPosition().x, getShape()->getPosition().y}},
    // };
    serialization["id"] = this->getId();
    serialization["type"] = this->getType();
    serialization["drawn"] = drawn;
    serialization["color"] = colorArray;
    serialization["solid"] = solid;
    serialization["shape"] = shapeArray;
    

    return serialization;
}

GameObject::GameObject(json serialization, sf::RenderWindow *window) {
    this->id = serialization["id"];
    this->type = serialization["type"];
    this->shape = new sf::RectangleShape(sf::Vector2f(serialization["shape"][0], serialization["shape"][1]));
    getShape()->setPosition(sf::Vector2f(serialization["shape"][2], serialization["shape"][3]));
    getShape()->setFillColor(sf::Color(serialization["color"][0], serialization["color"][1], serialization["color"][2], serialization["color"][3]));
    this->movementComponent = NULL;
    this->collisionComponent = new CollisionComponent(serialization["solid"]);
    if(serialization["drawn"]) {
        this->renderComponent = new RenderComponent(window);
    } else {
        this->renderComponent = NULL;
    }
}

json GameObject::serializeUpdate() {
    json update;
    update["id"] = getId();
    update["x"] = getShape()->getPosition().x;
    update["y"] = getShape()->getPosition().y;

    return update;
}

void GameObject::updateJson(json update) {
    getShape()->setPosition(sf::Vector2f(update["x"], update["y"]));
}