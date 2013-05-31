//=============================================================================
//File Name: Sounds.hpp
//Description: Provides a collection of sounds for use by other classes
//Author: Tyler Veness
//=============================================================================

#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class Sounds {
public:
    // @return a global instance of the resources available
    static Sounds* getInstance();

    sf::Music& background();
    sf::Sound& shoot();
    sf::Sound& enemyKill();
    sf::Sound& shipDamage();

protected:
    Sounds();

private:
    static Sounds* m_instance;

    static sf::Music m_background;

    static sf::SoundBuffer m_shootBuffer;
    static sf::Sound m_shoot;

    static sf::SoundBuffer m_shipDamageBuffer;
    static sf::Sound m_shipDamage;

    static sf::SoundBuffer m_enemyKillBuffer;
    static sf::Sound m_enemyKill;

    // disallow copy and assignment
    Sounds ( const Sounds& );
    void operator=( const Sounds& );
};

#endif // SOUNDS_HPP
