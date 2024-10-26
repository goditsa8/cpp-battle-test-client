#pragma once

#include <cstddef>
#include <vector>
#include <map>

#include "Component.hpp"


class GameObject final {
public:
    GameObject(uint64_t id) {
        this->id = id;
        gameObjects.push_back(this);
        gameObjectsMap.insert({id, this});
    }

    ~GameObject() {
        for (auto &&comp : components) {
            delete comp;
        }
        
    }

    std::uint64_t GetId() const { return id; }

    void AddComponent(Component* comp) {
        comp->gameObject = this;
        components.push_back(comp);
    }

    template<typename CompT>
    CompT* GetComponent() const {
        for (auto comp : components) {
            auto castedComp = dynamic_cast<CompT*>(comp);
            if (castedComp) {
                return castedComp;
            }
        }
        return nullptr;
    }

    void Update(uint64_t tick) {
        if (!IsEnabled()) {
            return;
        }

        gameTick = tick;
        for (auto &&comp : components) {
            if (comp->IsEnabled()) {
                comp->Update();
            }
        }
    }

    void Enable(bool val) {
        if (enabled == val) {
            return;
        }

        enabled = val;
        for (auto &&comp : components) {
            comp->Enable(enabled);
        }
    }

    bool IsEnabled() const { return enabled; }

    uint64_t GetGameTick() const { return gameTick; }
    static auto GetAll() { return gameObjects; }
    static GameObject* GetById(uint64_t id) { return gameObjectsMap[id]; }

private:
    std::uint64_t id;
    bool enabled = true;
    std::vector<Component*> components;

    inline static std::vector<GameObject*> gameObjects;
    inline static std::map<uint64_t, GameObject*> gameObjectsMap;

public:
    inline static uint64_t gameTick = 0;
};