#include "CollisionComponent.h"
#include "game-objects/GameObject.h"
#include "events/EventManager.h"
#include <nlohmann/json.hpp>
#include <v8.h>

using json = nlohmann::json;

CollisionComponent::CollisionComponent(bool scripted, std::string *scriptHandle) : ObjectComponent(scriptHandle)
{
    this->scripted = scripted;
}

void CollisionComponent::updateComponent() {
    //do nothing, behavior is handled by the collision method
}

sf::FloatRect CollisionComponent::getBounds() {
    return object->getShape()->getGlobalBounds();
}

bool CollisionComponent::hasScript() {

    return this->scripted;
}



bool CollisionComponent::collision(CollisionComponent *other) {
    // //local var for collision bounds
    if(other == NULL) {
        return false;
    }

        sf::FloatRect ourBounds = this->getBounds();
        sf::FloatRect theirBounds = other->getBounds();

        // if we overlap raise a collision event
    if(ourBounds.intersects(theirBounds)) {

        // v8::Local<v8::Context> *collision_context = new v8::Context::New()

        EventManager::getInstance()->raise( *(new Event(EventType::COLLISION, other)));
        
        return true;
    }
    return false;
}