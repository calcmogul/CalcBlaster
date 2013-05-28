//=============================================================================
//File Name: ShipBase.hpp
//Description: Controls Box2D ship
//Author: Tyler Veness
//=============================================================================

#ifndef SHIP_BASE_HPP
#define SHIP_BASE_HPP

#include "Box2DBase.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Texture.hpp>

class ShipBase : public Box2DBase {
public:
    ShipBase( const sf::Vector2f& position , float fullHealth );
    virtual ~ShipBase();

    // userData contains special data needed by each function impl
    virtual void controlShip( void* userData ) = 0;

    void setHealth( long long int health );

    long long int getHealth();

    sf::ConvexShape shape;

protected:
    // 100 per ship image
    long long int m_health;

    static float m_maxSpeed;
    b2Vec2 m_shipSpeed;
};

#endif // SHIP_BASE_HPP
