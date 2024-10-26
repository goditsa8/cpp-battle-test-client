#pragma once

#include "../Game/GameComponents.hpp"


class Game {
public:
    ~Game() {
        for (auto &&gameObject : GameObject::GetAll()) {
            delete gameObject;
        }
        
    }

	template<typename CompT>
	void UpdateAllComponents() {
		for (auto &&gameObject : GameObject::GetAll()) {
			if (!gameObject->IsEnabled()) {
				continue;
			}
			auto comp = gameObject->GetComponent<CompT>();
			if (comp) {
				comp->Update();
			}
		}
	}

	bool Update() {
		// put update order here
		UpdateAllComponents<Map>();
		UpdateAllComponents<Mappable>();
		UpdateAllComponents<Vision>();
		UpdateAllComponents<Melee>();
		UpdateAllComponents<Archer>();
		UpdateAllComponents<Damagable>();
		UpdateAllComponents<Movable>();

		GameObject::gameTick++;
		return true;
	}
};