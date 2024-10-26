#pragma once

#include "IO/System/EventLog.hpp"


class GameObject;

class Component {
    friend GameObject;

public:
    virtual void Enable(bool val) {
        if (enabled == val) {
            return;
        }

        enabled = val;
        enabled ? OnEnabled() : OnDisabled();
    }
    virtual void Update() {}

    bool IsEnabled() const { return enabled; }
    GameObject* GetGameObject() const { return gameObject; }

protected:
    virtual void OnEnabled() {}
    virtual void OnDisabled() {}

    inline static sw::EventLog eventLog;

private:
    GameObject* gameObject;
    bool enabled = true;
};