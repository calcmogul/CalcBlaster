// Copyright (c) Tyler Veness

#include "sounds.hpp"

Sounds* Sounds::m_instance = nullptr;

sf::Music Sounds::m_background{"resources/sounds/Background.flac"};
sf::SoundBuffer Sounds::m_shootBuffer{"resources/sounds/ShootFX.flac"};
sf::Sound Sounds::m_shoot{m_shootBuffer};
sf::SoundBuffer Sounds::m_enemyKillBuffer{"resources/sounds/EnemyKill.flac"};
sf::Sound Sounds::m_enemyKill{m_enemyKillBuffer};
sf::SoundBuffer Sounds::m_shipDamageBuffer{"resources/sounds/ShipDamage.flac"};
sf::Sound Sounds::m_shipDamage{m_shipDamageBuffer};

Sounds* Sounds::getInstance() {
  if (m_instance == nullptr) {
    m_instance = new Sounds;
  }

  return m_instance;
}

Sounds::Sounds() {
  m_background.setVolume(50.f);
  m_background.setLooping(true);

  m_shoot.setBuffer(m_shootBuffer);
  m_shoot.setVolume(50.f);

  m_shipDamage.setBuffer(m_shipDamageBuffer);

  m_enemyKill.setBuffer(m_enemyKillBuffer);
}

sf::Music& Sounds::background() {
  return m_background;
}

sf::Sound& Sounds::shoot() {
  return m_shoot;
}

sf::Sound& Sounds::shipDamage() {
  return m_shipDamage;
}

sf::Sound& Sounds::enemyKill() {
  return m_enemyKill;
}
