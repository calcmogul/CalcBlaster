//=============================================================================
//File Name: EnemyShip.hpp
//Description: Handles actions/AI and rendering of enemies
//Author: Tyler Veness
//=============================================================================

#ifndef ENEMY_SHIP_HPP
#define ENEMY_SHIP_HPP

#include "ShipBase.hpp"
#include <vector>

class EnemyShip : public ShipBase {
public:
    EnemyShip( const sf::Vector2f& position , float fullHealth );
    virtual ~EnemyShip();

    // Call this at end of program to free all planets and avoid memory leaks
    static void cleanup();

    static void drawAll( const ShipBase& ship , sf::RenderTarget& target , sf::RenderStates states = sf::RenderStates::Default );
    static void syncObjects( const sf::Window& referTo );

    static void add( const sf::Vector2f& position , float fullHealth );
    static void addBullet( unsigned int index , const sf::Window& referTo );

    static void controlShips( void* userData );

    static unsigned int size();

private:
    static sf::Texture m_shipTexture;
    static bool m_isLoaded;

    static std::vector<EnemyShip*> m_enemyShips;

    // userData contains a pointer to the player's ShipBase object
    void controlShip( void* userData );
};

#endif // ENEMY_SHIP_HPP
