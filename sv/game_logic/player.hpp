#pragma once

#include "gameObject.hpp"

class PlayerObject : public GameObject
{
private:
    /* data */
    Vector direction;
    int currentHealth;
    float speed;

public:
    PlayerObject(int maxHealth);
    ~PlayerObject();
    void update(float deltaTime, GameState &state) override;
    void onCollide(const GameObject *other) override;
};