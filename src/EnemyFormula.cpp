// Copyright (c) Tyler Veness

#include "EnemyFormula.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Sounds.hpp"
#include "Utils.hpp"

bool EnemyFormula::m_isLoaded = false;
std::vector<EnemyFormula*> EnemyFormula::m_enemyFormulas;
std::vector<sf::Texture*> EnemyFormula::m_textures;
std::vector<sf::Vector2u> EnemyFormula::m_sizes;
std::map<std::string, std::string> EnemyFormula::m_limits;

EnemyFormula::EnemyFormula(const sf::Vector2f& position, b2Vec2 speed)
    : ShipBase(position, 4, 1) {
  if (!m_isLoaded) {
    sf::Texture* tempTexture = nullptr;
    sf::Image* tempImage = new sf::Image;
    sf::Vector2u tempSize;

    std::stringstream ss;

    bool stillLoading = true;

    // Load limits for each file
    std::string line;
    unsigned int colonPos = 0;
    std::ifstream limitsFile("resources/functions/limits.txt");
    if (limitsFile.is_open()) {
      while (!limitsFile.eof()) {
        line.clear();
        std::getline(limitsFile, line);

        colonPos = line.find(":");

        /* Don't create an entry if there was no colon or there are no
         * characters after it
         */
        if (colonPos < line.length() - 1) {
          // Find next non-space character
          unsigned int typeStart = colonPos + 1;
          while (line[typeStart] == ' ' && typeStart < line.length()) {
            typeStart++;
          }

          // Extract type
          m_limits[line.substr(0, colonPos)] =
              line.substr(typeStart, line.length());
        }
      }

      limitsFile.close();
    }

    // While there are still functions in the folder, load them
    for (unsigned int i = 1; stillLoading; i++) {
      ss.clear();
      ss.str("");
      ss << "resources/functions/" << i << ".png";

      // We need a new texture for each image
      stillLoading = tempImage->loadFromFile(ss.str());

      if (stillLoading) {
        tempSize.x = tempImage->getSize().x;
        tempSize.y = tempImage->getSize().y;

        tempImage->createMaskFromColor(sf::Color(255, 255, 255), 0);

        tempTexture = new sf::Texture{
            sf::Vector2u{nextPowerTwo(tempSize.x), nextPowerTwo(tempSize.y)}};
        tempTexture->update(*tempImage);

        m_textures.push_back(tempTexture);
        m_sizes.push_back(tempSize);
      }
      /* Make sure to delete the last texture here because it won't be
       * referenced elsewhere due to the loading failure
       */
      else {
        delete tempTexture;
      }
    }

    m_isLoaded = true;
  }

  // If there are no images, exit
  if (m_textures.size() == 0) {
    std::exit(1);
  }

  // Choose a formula at random
  unsigned int imgPos = std::rand() % m_textures.size();
  m_formulaTexture = m_textures[imgPos];

  b2PolygonShape shipRectangle;

  // Store size locally to avoid unnecessary reads from graphics card
  sf::Vector2u tempSize = m_sizes[imgPos];
  sf::Vector2f shipSize(tempSize.x, tempSize.y);

  b2Vec2 shipVertices[4];
  shipVertices[0].Set(SFMLToBox_x(-shipSize.x / 2.f),
                      SFMLToBox_y(shipSize.y / 2.f, shipSize.y));
  shipVertices[1].Set(SFMLToBox_x(shipSize.x / 2.f),
                      SFMLToBox_y(shipSize.y / 2.f, shipSize.y));
  shipVertices[2].Set(SFMLToBox_x(shipSize.x / 2.f),
                      SFMLToBox_y(-shipSize.y / 2.f, shipSize.y));
  shipVertices[3].Set(SFMLToBox_x(-shipSize.x / 2.f),
                      SFMLToBox_y(-shipSize.y / 2.f, shipSize.y));

  shipRectangle.Set(shipVertices, 4);

  // Add the shape to the body.
  body->CreateFixture(&shipRectangle, 1.f);

  /* ===== Create SFML shape ===== */
  for (unsigned int i = 0; i < 4; i++) {
    shape.setPoint(i, sf::Vector2f(BoxToSFML_x(shipVertices[i].x),
                                   BoxToSFML_y(shipVertices[i].y,
                                               m_formulaTexture->getSize().y)));
  }
  shape.setOrigin(
      {0.f, static_cast<float>(m_textures[imgPos]->getSize().y + 1)});

  Box2DBase::setTexture(m_formulaTexture, tempSize);
  /* ============================= */

  // Stores speed of this formula
  m_speed = speed;

  // Find formula type string
  std::stringstream ss;
  ss << imgPos + 1 << ".png";
  std::string typeStr = m_limits[ss.str()];

  // Assign formula type corresponding to string to user data
  if (typeStr == std::string("zero")) {
    m_userData.formulaType = Bullet::zero;
  } else if (typeStr == std::string("constant")) {
    m_userData.formulaType = Bullet::constant;
  } else if (typeStr == std::string("infinity")) {
    m_userData.formulaType = Bullet::infinity;
  } else {
    std::cout << "Unknown type: \"" << typeStr << "\"\n";
    m_userData.formulaType = Bullet::infinity;
  }

  // Assign formula object pointer to user data
  m_userData.formulaObj = this;

  // Assign user data to Box2D body
  body->GetUserData().pointer = reinterpret_cast<uintptr_t>(&m_userData);
}

EnemyFormula::~EnemyFormula() {
  std::vector<EnemyFormula*>::iterator index;
  for (index = m_enemyFormulas.begin(); *index != this; index++) {
    if (index >= m_enemyFormulas.end()) {
      return;
    }
  }

  m_enemyFormulas.erase(index);
}

void EnemyFormula::cleanup() {
  for (unsigned int index = m_enemyFormulas.size(); index > 0; index--) {
    // The object's destructor will remove the object from the array
    delete m_enemyFormulas[0];
  }

  for (unsigned int index = m_textures.size(); index > 0; index--) {
    /* We must remove the object from the array explicitly because the
     * sf::Texture destructor won't do it for us
     */
    delete m_textures[0];
    m_textures.erase(m_textures.begin());

    m_sizes.erase(m_sizes.begin());
  }
}

void EnemyFormula::drawAll([[maybe_unused]] const ShipBase& ship,
                           sf::RenderTarget& target,
                           [[maybe_unused]] sf::RenderStates states) {
  for (unsigned int index = 0; index < m_enemyFormulas.size(); index++) {
    // Redraw formula
    target.draw(*m_enemyFormulas[index]);
  }
}

void EnemyFormula::syncObjects(const sf::Window& referTo) {
  for (unsigned int index = 0; index < m_enemyFormulas.size(); index++) {
    m_enemyFormulas[index]->syncObject(referTo);
  }
}

void EnemyFormula::add(const sf::Vector2f& position, b2Vec2 speed) {
  m_enemyFormulas.push_back(new EnemyFormula(position, speed));
}

void EnemyFormula::addBullet(unsigned int index, const sf::Window& referTo) {
  Bullet::add(*m_enemyFormulas[index], referTo, sf::Color(0, 0, 255),
              Bullet::constant);
}

void EnemyFormula::checkCollisions(ShipBase& ship,
                                   const sf::RenderWindow& referTo) {
  EnemyFormula* formula;

  unsigned int i = 0;
  while (i < m_enemyFormulas.size()) {
    formula = m_enemyFormulas[i];

    sf::Vector2f pos(formula->drawShape->getPosition());

    // If bullet is off left or right of screen
    if (pos.x < referTo.getView().getCenter().x - referTo.getSize().x / 2.f ||
        pos.x > referTo.getView().getCenter().x + referTo.getSize().x / 2.f) {
      delete formula;  // bullet is outside of window view so delete it

      /* Deal damage to the player since they didn't shoot the formula in
       * time.
       */
      ship.setHealth(ship.getHealth() - 50);

      // Play sound for ship damage
      Sounds::getInstance()->shipDamage().play();

      continue;  // stop checking body collisions for this bullet because it
                 // doesn't exist anymore
    }

    // If bullet is off bottom of screen
    if (pos.y > referTo.getView().getCenter().y + referTo.getSize().y / 2.f) {
      delete formula;  // bullet is outside of window view so delete it

      /* Deal damage to the player since they didn't shoot the formula in
       * time.
       */
      ship.setHealth(ship.getHealth() - 50);

      // Play sound for ship damage
      Sounds::getInstance()->shipDamage().play();

      continue;  // stop checking body collisions for this bullet because it
                 // doesn't exist anymore
    }

    i++;
  }
}

void EnemyFormula::controlEnemies(void* userData) {
  for (unsigned int index = 0; index < m_enemyFormulas.size(); index++) {
    m_enemyFormulas[index]->controlShip(userData);
  }
}

void EnemyFormula::controlShip([[maybe_unused]] void* userData) {
  // Adjust velocity to one given upon construction
  body->SetLinearVelocity(m_speed);
}
