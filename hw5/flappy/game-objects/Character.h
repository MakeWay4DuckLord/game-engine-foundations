#pragma once
#include "game-objects/GameObject.h"
#include "game-objects/components/CharMoveComponent.h"
#include <utility>
#include <v8.h>

// class GameObject;

class Character : public GameObject {
    public:
        Character(std::string id, sf::Shape *shape, float speed, sf::RenderWindow *window, GameObject *spawnPoint);
        void respawn();
        void setSpawnPoint(GameObject *spawnPoint);
		v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name="default");
    private:
        GameObject *spawnPoint;
        CharMoveComponent *charMoveComp;
        bool sprinting;
		
        static void setSprint(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void isSprinting(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
};