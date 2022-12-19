#include "Character.h"
#include "GameObject.h"
// #include "components/CharMoveComponent.h"
#include "components/CollisionComponent.h"
#include "components/RenderComponent.h"
#include <filesystem>
#include <cstring>
#include "scripting/ScriptManager.h"
#include "scripting/v8helpers.h"

Character::Character(std::string id, sf::Shape *shape, float speed, sf::RenderWindow *window, GameObject *spawnPoint) : GameObject(id, TYPE_CHARACTER, shape, new CharMoveComponent(speed), new CollisionComponent(false), new RenderComponent(window)) {
    this->setSpawnPoint(spawnPoint);
    this->respawn();
}

void Character::respawn() {
    this->getShape()->setPosition(this->spawnPoint->getShape()->getPosition());
}

void Character::setSpawnPoint(GameObject *spawnPoint) {
    this->spawnPoint = spawnPoint;
}

// /**
//  * IMPORTANT: Pay close attention to the definition of the std::vector in this
//  * example implementation. The v8helpers::expostToV8 will assume you have
//  * instantiated this exact type of vector and passed it in. If you don't the
//  * helper function will not work. 
//  */
// v8::Local<v8::Object> Character::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
// {
// 	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
// 	v.push_back(v8helpers::ParamContainer("sprinting", isSprinting, setSprint));
// 	return v8helpers::exposeToV8(id, this, v, isolate, context, context_name);
// }