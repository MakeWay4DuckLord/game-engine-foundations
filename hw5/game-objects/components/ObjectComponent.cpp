#include "ObjectComponent.h"
#include "game-objects/GameObject.h"
ObjectComponent::ObjectComponent(void *data) {
    this->data = data;
}

void ObjectComponent::setObject(GameObject *object) {
    this->object = object;
}