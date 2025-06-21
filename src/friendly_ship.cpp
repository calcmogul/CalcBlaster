// Copyright (c) Tyler Veness

#include "friendly_ship.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Keyboard.hpp>

sf::Texture FriendlyShip::m_shipTexture;
bool FriendlyShip::m_isLoaded = false;

FriendlyShip::FriendlyShip(const sf::Vector2f& position, float fullHealth)
    : ShipBase(position, 6, fullHealth) {
  if (!m_isLoaded) {
    sf::Image shipImage{"resources/GalagaShip.png"};

    if (!m_shipTexture.loadFromImage(
            shipImage, false,
            sf::IntRect({1, 0}, {static_cast<int>(shipImage.getSize().x - 2),
                                 static_cast<int>(shipImage.getSize().y)}))) {
      std::exit(1);
    }

    m_isLoaded = true;
  }

  m_shipTexture.setSmooth(true);

  b2PolygonShape shipTriangle;

  b2Vec2
      shipVertices[6];  // all shifted left by 15 pixels and down by 20 pixels
  shipVertices[0].Set(-1.f / 30.f, (m_shipTexture.getSize().y - 20.f) / 30.f);
  shipVertices[1].Set(-15.f / 30.f, (m_shipTexture.getSize().y - 36.f) / 30.f);
  shipVertices[2].Set(-15.f / 30.f, (m_shipTexture.getSize().y - 51.f) / 30.f);
  shipVertices[3].Set(14.f / 30.f, (m_shipTexture.getSize().y - 51.f) / 30.f);
  shipVertices[4].Set(14.f / 30.f, (m_shipTexture.getSize().y - 36.f) / 30.f);
  shipVertices[5].Set(0.f / 30.f, (m_shipTexture.getSize().y - 20.f) / 30.f);

  shipTriangle.Set(shipVertices, 6);

  // Add the shape to the body.
  body->CreateFixture(&shipTriangle, 1.f);

  /* ===== Create SFML shape ===== */
  for (unsigned int index = 0; index < 6; index++) {
    shape.setPoint(index, sf::Vector2f(shipVertices[index].x * 30.f,
                                       m_shipTexture.getSize().y -
                                           shipVertices[index].y * 30.f));
  }
  shape.setOrigin({0.f, static_cast<float>(m_shipTexture.getSize().y)});

  shape.setTexture(&m_shipTexture);
  /* ============================= */
}

FriendlyShip::~FriendlyShip() {}

void FriendlyShip::controlShip([[maybe_unused]] void* userData) {
#if 1
  // Sets base velocity to forward and current body angle to straight
  body->SetLinearVelocity(b2Vec2(0.f, 2.f));
  body->SetTransform(body->GetPosition(), 0.f);

  // Holds current velocity
  b2Vec2 curVel = body->GetLinearVelocity();

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
    curVel += b2Vec2(-6.f, 0.f);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
    curVel += b2Vec2(6.f, 0.f);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
    curVel += b2Vec2(0.f, 6.f);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
    curVel += b2Vec2(0.f, -6.f);
  }

  // Set resultant velocity
  body->SetLinearVelocity(curVel);
#else
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
    body->SetTransform(
        body->GetPosition(),
        body->GetAngle() + 4.f * std::numbers::pi_v<float> / 180.f);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
    body->SetTransform(
        body->GetPosition(),
        body->GetAngle() - 4.f * std::numbers::pi_v<float> / 180.f);
  }

  // limit body angle to between 0 and 2 * pi
  float tempAngle = body->GetAngle();
  if (tempAngle > 2.f * std::numbers::pi_v<float>) {
    body->SetTransform(body->GetPosition(),
                       tempAngle - 2.f * std::numbers::pi_v<float>);
  } else if (tempAngle < 0.f) {
    body->SetTransform(body->GetPosition(),
                       tempAngle + 2.f * std::numbers::pi_v<float>);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
    body->ApplyForceToCenter(
        2.f * b2Vec2(7.5f * std::cos(body->GetAngle() +
                                     std::numbers::pi_v<float> / 2.f),
                     7.5f * std::sin(body->GetAngle() +
                                     std::numbers::pi_v<float> / 2)),
        true);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
    body->ApplyForceToCenter(
        2.f * b2Vec2(-7.5f * std::cos(body->GetAngle() +
                                      std::numbers::pi_v<float> / 2.f),
                     -7.5f * std::sin(body->GetAngle() +
                                      std::numbers::pi_v<float> / 2)),
        true);
  }
#endif
}
