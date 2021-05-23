#pragma once

#include "gameObject.hpp"

class BulletObject : public GameObject
{
private:
    /* data */
    Vector direction;
    int damage;
    float speed;

public:
    BulletObject(int damage, Vector v);
    ~BulletObject();
    void update(float deltaTime, GameState &state) override;
};
