#pragma once
#include "gameState.hpp"

struct Vector
{
    float x;
    float y;

public:
    Vector() : x(0.0f), y(0.0f) {}
    Vector(float x, float y) : x(x), y(y) {}
    friend Vector operator+(const Vector &a, const Vector &b)
    {
        return Vector(a.x + b.x, a.y + b.y);
    }
    void operator+=(const Vector &other)
    {
        x = x + other.x;
        y = y + other.y;
    }
    friend Vector operator*(const Vector &a, float b)
    {
        return Vector(a.x * b, a.y * b);
    }
};

class GameObject
{
protected:
    /* data */
    Vector position;

public:
    virtual void update(float deltaTime, GameState &state) = 0;
    virtual void onCollide(const GameObject *other){};
    Vector getPosition() { return position; };
};
