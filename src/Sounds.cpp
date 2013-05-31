//=============================================================================
//File Name: Sounds.cpp
//Description: Provides a collection of fonts for use by other classes
//Author: Tyler Veness
//=============================================================================

#include "Sounds.hpp"

Sounds* Sounds::m_instance = NULL;

sf::Music Sounds::m_background;
sf::SoundBuffer Sounds::m_shootBuffer;
sf::Sound Sounds::m_shoot;
sf::SoundBuffer Sounds::m_enemyKillBuffer;
sf::Sound Sounds::m_enemyKill;
sf::SoundBuffer Sounds::m_shipDamageBuffer;
sf::Sound Sounds::m_shipDamage;

Sounds* Sounds::getInstance() {
    if ( m_instance == NULL ) {
        m_instance = new Sounds;
    }

    return m_instance;
}

Sounds::Sounds() {
    m_background.openFromFile( "Resources/Sounds/Background.flac" );
    m_background.setVolume( 50.f );

    m_shootBuffer.loadFromFile( "Resources/Sounds/ShootFX.flac" );
    m_shoot.setBuffer( m_shootBuffer );
    m_shoot.setVolume( 50.f );

    m_shipDamageBuffer.loadFromFile( "Resources/Sounds/ShipDamage.flac" );
    m_shipDamage.setBuffer( m_shipDamageBuffer );

    m_enemyKillBuffer.loadFromFile( "Resources/Sounds/EnemyKill.flac" );
    m_enemyKill.setBuffer( m_enemyKillBuffer );
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
