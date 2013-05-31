//=============================================================================
//File Name: ShipBase.cpp
//Description: Controls Box2D ship
//Author: Tyler Veness
//=============================================================================

#include <SFML/Graphics/Image.hpp>
#include "ShipBase.hpp"

float ShipBase::m_maxSpeed = 30.f;

ShipBase::ShipBase( const sf::Vector2f& position , unsigned int pts , long long int fullHealth ) : Box2DBase( &shape , position , b2_dynamicBody ) , shape( pts ) {
    m_health = fullHealth;
    m_score = 0.f;

    body->SetLinearVelocity( b2Vec2( 0.f , 2.f ) );
}

ShipBase::~ShipBase() {

}

void ShipBase::setHealth( long long int health ) {
    m_health = health;
}

long long int ShipBase::getHealth() const {
    return m_health;
}

void ShipBase::setScore( unsigned long long int score ) {
    m_score = score;
}

unsigned long long int ShipBase::getScore() const {
    return m_score;
}
