#include "bullet.hpp"

BulletObject::BulletObject(int damage, Vector v) : damage(damage), direction(v), speed(10) {}

BulletObject::~BulletObject()
{
}

void BulletObject::update(float deltaTime, GameState &state)
{
    position += direction * deltaTime * speed;
}