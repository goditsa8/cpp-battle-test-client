#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <map>

#include "Core/GameObject.hpp"
#include "Map.hpp"


class Mappable : public Component { // someone who is able to be on the map :)
public:
    Mappable(GameObject* mapObj, uint32_t x, uint32_t y) : mapObj(map), x(x), y(y) {
        Map* map = mapObj->GetComponent<Map>();
        map->GetSquare(x, y).unit = GetGameObject();
    }

    GameObject* GetMapObject() const { return mapObj; }
    uint32_t GetX() const { return x; }
    uint32_t GetY() const { return y; }

    void SetCell(uint32_t x, uint32_t y) {
        mapObj->GetComponent<Map>()->GetSquare(this->x, this->y).unit = nullptr;
        mapObj->GetComponent<Map>()->GetSquare(x, y).unit = GetGameObject();
        this->x = x;
        this->y = y;
    }

    void OnDisabled() override {
        mapObj->GetComponent<Map>()->GetSquare(x, y).unit = nullptr;
    }

private:
    GameObject* mapObj;
    uint32_t x, y;
};


class Damagable : public Component {
public:
    explicit Damagable(int hp) : hp(hp) {}

    void Damage(GameObject* attacker, uint32_t dmg) {
        if (hp >= dmg) {
            hp -= dmg;
        }

        uint32_t atkId = attacker->GetId();
        uint32_t targetId = GetGameObject()->GetId();
        eventLog.log(GetGameObject()->GetGameTick(), sw::io::UnitAttacked{atkId, targetId, dmg, hp});
    }

    void Update() override {
        if (hp <= 0) {
            Die();
        }
    }

    bool IsAlive() const { return hp > 0; }
    bool IsDead() const { return !IsAlive(); }

    static bool IsSomeoneAlive() {
        for (auto &&gameObject : GameObject::GetAll()) {
            auto damagable = gameObject->GetComponent<Damagable>();
            if (!damagable) {
                continue;
            }

            if (damagable->IsAlive()) {
                return true;
            }
        }
        return false;
    }

private:
    uint32_t hp = 0;

    void Die() {
        GetGameObject()->Enable(false);
        uint32_t id = GetGameObject()->GetId();
        eventLog.log(GetGameObject()->GetGameTick(), sw::io::UnitDied{id});
    }
};


class Vision : public Component {
public:
    std::vector<GameObject*> GetVisibleEnimies(uint32_t closeRange, uint32_t farRange) {
        assert(closeRange < farRange);

        int currentX = GetGameObject()->GetComponent<Mappable>()->GetX();
        int currentY = GetGameObject()->GetComponent<Mappable>()->GetY();

        std::vector<GameObject*> enemiesOnSite;
        for (auto &&gameObject : GameObject::GetAll()) {
            if (!gameObject->IsEnabled() || !gameObject->GetComponent<Damagable>() || GetGameObject() == gameObject) {
                continue;
            }

            Mappable* mappable = gameObject->GetComponent<Mappable>();
            int enemyX = mappable->GetX();
            int enemyY = mappable->GetY();
            int diffX = std::abs(enemyX - currentX);
            int diffY = std::abs(enemyY - currentY);
            int diffMax = std::max(diffX, diffY);
            if (diffMax >= closeRange && diffMax <= farRange) {
                enemiesOnSite.push_back(gameObject);
            }
        }
        return enemiesOnSite;
    }
};


class Warrior : public Component {
public:
    bool IsAttacking() const { return isAttacking; }

protected:
    bool isAttacking = false;
};


class Melee : public Warrior {
public:
    explicit Melee(uint32_t strength) : strength(strength) {}

    void Update() override {
        isAttacking = false;
        if (GetGameObject()->GetComponent<Damagable>()->IsDead()) {
            return;
        }

        Vision* vision = GetGameObject()->GetComponent<Vision>();
        for (auto &&enemy : vision->GetVisibleEnimies(0, 1)) {
            enemy->GetComponent<Damagable>()->Damage(GetGameObject(), strength);
            isAttacking = true;
        }
    }

private:
    uint32_t strength = 0;
};


class Archer : public Warrior {
public:
    Archer(uint32_t agility, uint32_t closeRange, uint32_t farRange) : agility(agility), closeRange(closeRange), farRange(farRange) {}

    void Update() override {
        isAttacking = false;
        if (GetGameObject()->GetComponent<Damagable>()->IsDead()) {
            return;
        }
        
        Vision* vision = GetGameObject()->GetComponent<Vision>();
        for (auto &&enemy : vision->GetVisibleEnimies(closeRange, farRange)) {
            enemy->GetComponent<Damagable>()->Damage(GetGameObject(), agility);
            isAttacking = true;
        }
    }

private:
    uint32_t agility = 0;
    uint32_t closeRange = 0;
    uint32_t farRange = 0;
};


class Movable : public Component {
public:
    void March(uint32_t targetX, uint32_t targetY) {
        wantMove = true;
        this->targetX = targetX;
        this->targetY = targetY;

        uint32_t id = GetGameObject()->GetId();
        uint32_t x = GetGameObject()->GetComponent<Mappable>()->GetX();
        uint32_t y = GetGameObject()->GetComponent<Mappable>()->GetY();
    }

    static bool WantSomeoneMove() {
        for (auto &&gameObject : GameObject::GetAll()) {
            auto movable = gameObject->GetComponent<Movable>();
            if (!movable) {
                continue;
            }

            if (movable->WantMove()) {
                return true;
            }
        }
        return false;
    }

    bool WantMove() const { return wantMove; }

    void Update() override {
        Component::Update();

        if (!WantMove()) {
            return;
        }

        if (GetGameObject()->GetComponent<Warrior>()->IsAttacking()) {
            return;
        }

        Mappable* mappable = GetGameObject()->GetComponent<Mappable>();
        int moveDirX = mappable->GetX() - targetX;
        int moveDirY = mappable->GetY() - targetY;
        if (moveDirX == 0 && moveDirY == 0) {
            wantMove = false;
            uint32_t id = GetGameObject()->GetId();
            uint32_t x = GetGameObject()->GetComponent<Mappable>()->GetX();
            uint32_t y = GetGameObject()->GetComponent<Mappable>()->GetY();
            eventLog.log(GetGameObject()->GetGameTick(), sw::io::MarchEnded{id, x, y});
            return;
        }

        wantMove = true;
        int actualMoveX, actualMoveY;
        int moveDirXAbs = std::abs(moveDirX);
        int moveDirYAbs = std::abs(moveDirY);
        if (moveDirXAbs == moveDirYAbs) {   // diagonal move
            actualMoveX = moveDirX / moveDirXAbs;
            actualMoveY = moveDirY / moveDirYAbs;
        } else if (moveDirXAbs > moveDirYAbs) { // horizontal move
            actualMoveX = moveDirX / moveDirXAbs;
            actualMoveY = 0;
        } else {    // vertical move
            actualMoveX = 0;
            actualMoveY = moveDirY / moveDirYAbs;
        }

        uint32_t newSquareX = mappable->GetX() - actualMoveX;
        uint32_t newSquareY = mappable->GetY() - actualMoveY;

        // check bad squares
        Map* map = mappable->GetMapObject()->GetComponent<Map>();
        if (!map->IsValidSquare(newSquareX, newSquareY)) {
            wantMove = false;
            return;
        }

        // check occupied squares
        if (!map->GetSquare(newSquareX, newSquareY).IsEmpty()) {
            return;
        }

        mappable->SetCell(newSquareX, newSquareY);

        uint32_t id = GetGameObject()->GetId();
        eventLog.log(GetGameObject()->GetGameTick(), sw::io::UnitMoved{id, newSquareX, newSquareY});
    }

    void OnDisabled() override {
        wantMove = false;
    }

private:
    uint32_t targetX = 0;
    uint32_t targetY = 0;

    bool wantMove = false;
};