#pragma once

#include "GameComponents.hpp"

inline void SpawnWarrior(int id, int x, int y, int hp, int strength) {
    auto warrior = new GameObject(id);
    warrior->AddComponent(new Mappable(GetMap(), x, y));
    warrior->AddComponent(new Vision());
    warrior->AddComponent(new Melee(strength));
    warrior->AddComponent(new Damagable(hp));
    warrior->AddComponent(new Movable());
}

inline void SpawnArcher(int id, int x, int y, int hp, int agility, int strength, int range) {
    auto archer = new GameObject(id);
    archer->AddComponent(new Mappable(GetMap(), x, y));
    archer->AddComponent(new Vision());
    archer->AddComponent(new Melee(strength));
    archer->AddComponent(new Archer(agility, 2, range));
    archer->AddComponent(new Damagable(hp));
    archer->AddComponent(new Movable());
}