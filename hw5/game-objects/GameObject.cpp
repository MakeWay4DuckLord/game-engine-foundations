#include "GameObject.h"
#include "components/MovementComponent.h"
#include "components/CollisionComponent.h"
#include "components/RenderComponent.h"
#include <SFML/Graphics/Shape.hpp>
// #include <SFML/Graphics.hpp>
// #include <vector>
#include "scripting/v8helpers.h"


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






void GameObject::getShapeCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void *ptr = wrap->Value();
    //so far this is the same as examples

    //now we can get the shape, instead of an int
    sf::Shape *shape = static_cast<GameObject*>(ptr)->getShape();

    //create a json object from the shape
    json shape_j;
    shape_j["height"] = shape->getGlobalBounds().height;
    shape_j["width"] = shape->getGlobalBounds().width;
    shape_j["x"] = shape->getPosition().x;
    shape_j["y"] = shape->getPosition().y;

    //return that json object as a v8 string
    std::string dump = shape_j.dump();
	v8::Local<v8::String> v8_dump = v8::String::NewFromUtf8(info.GetIsolate(), dump.c_str(), v8::String::kNormalString);
    info.GetReturnValue().Set(v8_dump);
}


void GameObject::setShapeCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void *ptr = wrap->Value();
    v8::String::Utf8Value utf8_dump(info.GetIsolate(), value->ToString());
    static_cast<GameObject*>(ptr)->updateJson(json::parse(*utf8_dump));
}

v8::Local<v8::Object> GameObject::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name) {
    // std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;

    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
    v.push_back(v8helpers::ParamContainer("shape", getShapeCallback, setShapeCallback));
    // v.push_back(v8helpers::ParamContainer("x", getGameObjectX, setGameObjectX));

    return v8helpers::exposeToV8(id, this, v, isolate, context, context_name);
}

v8::Local<v8::Object> GameObject::exposeToV8(std::string name,v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name) {
    // std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;

    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
    v.push_back(v8helpers::ParamContainer("shape", getShapeCallback, setShapeCallback));
    // v.push_back(v8helpers::ParamContainer("x", getGameObjectX, setGameObjectX));

    return v8helpers::exposeToV8(name, this, v, isolate, context, context_name);
}

