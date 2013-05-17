//=============================================================================
//File Name: Ship.cpp
//Description: Controls Box2D ship
//Author: Tyler Veness
//=============================================================================

#include <SFML/Graphics/Image.hpp>
#include "Ship.hpp"

sf::Texture Ship::m_shipTexture;
bool Ship::m_isLoaded = false;
float Ship::m_maxSpeed = 30.f;

Ship::Ship( const sf::Vector2f& position , float fullHealth ) : Box2DBase( &shape , position , b2_dynamicBody ) , shape( 6 ) {
    if ( !m_isLoaded ) {
        sf::Image shipImage;
        if ( !shipImage.loadFromFile( "Resources/GalagaShip.png" ) ) {
            exit( 1 );
        }

        if ( !m_shipTexture.loadFromImage( shipImage ) ) {
            exit( 1 );
        }

        m_isLoaded = true;
    }

    m_health = fullHealth;

    b2PolygonShape shipTriangle;

    b2Vec2 shipVertices[6]; // all shifted left by 15 pixels and down by 20 pixels
    shipVertices[0].Set( -1.f / 30.f , (m_shipTexture.getSize().y - 20.f) / 30.f );
    shipVertices[1].Set( -15.f / 30.f , (m_shipTexture.getSize().y - 36.f) / 30.f );
    shipVertices[2].Set( -15.f / 30.f , (m_shipTexture.getSize().y - 51.f) / 30.f );
    shipVertices[3].Set( 14.f / 30.f , (m_shipTexture.getSize().y - 51.f) / 30.f );
    shipVertices[4].Set( 14.f / 30.f , (m_shipTexture.getSize().y - 36.f) / 30.f );
    shipVertices[5].Set( 0.f / 30.f , (m_shipTexture.getSize().y - 20.f) / 30.f );

    shipTriangle.Set( shipVertices , 6 );

    // Add the shape to the body.
    body->CreateFixture( &shipTriangle , 1.f );

    /* ===== Create SFML shape ===== */
    for ( unsigned int index = 0 ; index < 6 ; index++ ) {
        shape.setPoint( index , sf::Vector2f( shipVertices[index].x * 30.f , m_shipTexture.getSize().y - shipVertices[index].y * 30.f ) );
    }
    shape.setOrigin( 0 , m_shipTexture.getSize().y );

    shape.setTexture( &m_shipTexture );
    /* ============================= */
}

Ship::~Ship() {

}

void Ship::controlShip() {
    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) ) {
        body->SetAngularVelocity( body->GetAngularVelocity() + 0.4f );
    }

    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) ) {
        body->SetAngularVelocity( body->GetAngularVelocity() - 0.4f );
    }

    // limit body angle to between 0 and 2 * pi
    float tempAngle = body->GetAngle();
    if ( tempAngle > 2.f * b2_pi ) {
        body->SetTransform( body->GetPosition() , tempAngle - 2.f * b2_pi );
    }
    else if ( tempAngle < 0.f ) {
        body->SetTransform( body->GetPosition() , tempAngle + 2.f * b2_pi );
    }

    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) ) {
        body->ApplyForceToCenter( 4 * b2Vec2( 7.5f * cos( body->GetAngle() + b2_pi / 2.f ) , 7.5f * sin( body->GetAngle() + b2_pi / 2 ) ) );
    }

    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) ) {
        body->ApplyForceToCenter( 4 * b2Vec2( -7.5f * cos( body->GetAngle() + b2_pi / 2.f ) , -7.5f * sin( body->GetAngle() + b2_pi / 2 ) ) );
    }

    m_shipSpeed = body->GetLinearVelocity();
    if ( m_shipSpeed.Length() > m_maxSpeed ) {
        float32 angle = atan2( m_shipSpeed.y , m_shipSpeed.x );

        body->SetLinearVelocity( b2Vec2( m_shipSpeed.x - ( m_shipSpeed.Length() - m_maxSpeed ) * cos( angle ) , m_shipSpeed.y - ( m_shipSpeed.Length() - m_maxSpeed ) * sin( angle ) ) );
    }
}

float Ship::getHealth() {
    return m_health;
}
