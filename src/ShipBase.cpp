// Copyright (c) Tyler Veness

#include "ShipBase.hpp"

#include <SFML/Graphics/Image.hpp>

float ShipBase::m_maxSpeed = 30.f;

ShipBase::ShipBase(const sf::Vector2f& position, uint32_t pts,
                   int64_t fullHealth)
    : Box2DBase(&shape, position, b2_dynamicBody), shape(pts) {
  m_health = fullHealth;
  m_score = 0.f;

  body->SetLinearVelocity(b2Vec2(0.f, 2.f));
}

ShipBase::~ShipBase() {}

void ShipBase::setHealth(int64_t health) {
  m_health = health;
}

int64_t ShipBase::getHealth() const {
  return m_health;
}

void ShipBase::setScore(uint64_t score) {
  m_score = score;
}

uint64_t ShipBase::getScore() const {
  return m_score;
}
