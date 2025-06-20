// Copyright (c) Tyler Veness

#pragma once

#include <map>
#include <string>
#include <vector>

#include "bullet.hpp"
#include "ship_base.hpp"

class EnemyFormula;

struct userData {
  Bullet::BulletType formulaType;

  EnemyFormula* formulaObj;
};

/**
 * Handles actions/AI and rendering of enemy formulas.
 */
class EnemyFormula : public ShipBase {
 public:
  EnemyFormula(const sf::Vector2f& position, b2Vec2 speed);
  virtual ~EnemyFormula();

  // Call this at end of program to free all planets and avoid memory leaks
  static void cleanup();

  static void drawAll(const ShipBase& ship, sf::RenderTarget& target,
                      sf::RenderStates states = sf::RenderStates::Default);
  static void syncObjects(const sf::Window& referTo);

  static void add(const sf::Vector2f& position, b2Vec2 speed);
  static void addBullet(unsigned int index, const sf::Window& referTo);

  // Destroys object if it went off the screen
  static void checkCollisions(ShipBase& ship, const sf::RenderWindow& referTo);

  static void controlEnemies(void* userData);

 private:
  static bool m_isLoaded;

  static std::vector<EnemyFormula*> m_enemyFormulas;

  // Contains textures for images of all formulas
  static std::vector<sf::Texture*> m_textures;

  // Contains sizes of images which are contained by the textures
  static std::vector<sf::Vector2u> m_sizes;

  static std::map<std::string, std::string> m_limits;

  sf::Texture* m_formulaTexture;

  b2Vec2 m_speed;

  // Bullets use this data to determine what to do with bodies they hit
  userData m_userData;

  void controlShip(void* userData);
};
