// Copyright (c) Tyler Veness

#pragma once

#include "ShipBase.hpp"

/**
 * Handles actions and rendering of friendly ships (including player).
 */
class FriendlyShip : public ShipBase {
 public:
  FriendlyShip(const sf::Vector2f& position, float fullHealth);
  virtual ~FriendlyShip();

  void controlShip(void* userData);

 private:
  static sf::Texture m_shipTexture;
  static bool m_isLoaded;
};
