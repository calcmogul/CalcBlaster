//=============================================================================
//File Name: ShipBase.cpp
//Description: Controls Box2D ship
//Author: Tyler Veness
//=============================================================================

#include <SFML/Graphics/Image.hpp>
#include "ShipBase.hpp"

float ShipBase::m_maxSpeed = 30.f;

ShipBase::ShipBase( const sf::Vector2f& position , float fullHealth ) : Box2DBase( &shape , position , b2_dynamicBody ) , shape( 6 ) {
    m_health = fullHealth;

    body->SetLinearVelocity( b2Vec2( 0.f , 10.f ) );
}

ShipBase::~ShipBase() {

}

void ShipBase::setHealth( long long int health ) {
    m_health = health;
}

long long int ShipBase::getHealth() {
    return m_health;
}
