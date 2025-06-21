// Copyright (c) Tyler Veness

#pragma once

#include <stdint.h>

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "box2d_base.hpp"

/**
 * Controls Box2D ship.
 */
class ShipBase : public Box2DBase {
 public:
  // 'pts' is number of points in convex shape
  ShipBase(const sf::Vector2f& position, uint32_t pts, int64_t fullHealth);
  virtual ~ShipBase();

  // userData contains special data needed by each function impl
  virtual void controlShip(void* userData) = 0;

  void setHealth(int64_t health);

  int64_t getHealth() const;

  void setScore(uint64_t score);

  uint64_t getScore() const;

  sf::ConvexShape shape;

 protected:
  // 100 per ship image
  int64_t m_health;

  uint64_t m_score;

  static float m_maxSpeed;
  b2Vec2 m_shipSpeed;
};
