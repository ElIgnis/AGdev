#include "Entity.h"

Entity::Entity()
: direction(0, 0, 1)
, health(0)
, lives(0)
, mass(1.f)
, angle(0.f)
, moveSpeed(150.f)
, jumpSpeed(10.f)
, gravity(9.8f)
, orientation(0.f)
, moveSpeed_Mult(1.f)
, inAir(false)
, moving(false)
{
}


Entity::~Entity()
{
}