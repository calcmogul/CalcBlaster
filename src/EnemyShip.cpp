//=============================================================================
//File Name: Enemy.hpp
//Description: Handles actions/AI and rendering of enemies
//Author: Tyler Veness
//=============================================================================

#include "EnemyShip.hpp"
#include "Bullet.hpp"
#include <SFML/Window/Keyboard.hpp>

sf::Texture EnemyShip::m_shipTexture;
bool EnemyShip::m_isLoaded = false;
std::vector<EnemyShip*> EnemyShip::m_enemyShips;

EnemyShip::EnemyShip( const sf::Vector2f& position , float fullHealth ) : ShipBase( position , fullHealth ) {
    if ( !m_isLoaded ) {
        sf::Image shipImage;
        if ( !shipImage.loadFromFile( "Resources/CapturedGalagaShip.png" ) ) {
            exit( 1 );
        }

        if ( !m_shipTexture.loadFromImage( shipImage ) ) {
            exit( 1 );
        }

        m_isLoaded = true;
    }

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

EnemyShip::~EnemyShip() {
    std::vector<EnemyShip*>::iterator index;
    for ( index = m_enemyShips.begin() ; *index != this ; index++ ) {
        if ( index >= m_enemyShips.end() ) {
            return;
        }
    }

    m_enemyShips.erase( index );
}

void EnemyShip::cleanup() {
    for ( unsigned int index = m_enemyShips.size() ; index > 0 ; index-- ) {
        delete m_enemyShips[0];
    }
}

void EnemyShip::drawAll( const ShipBase& ship , sf::RenderTarget& target , sf::RenderStates states ) {
    for ( unsigned int index = 0 ; index < m_enemyShips.size() ; index++ ) {
        // Redraw enemy ship
        target.draw( *m_enemyShips[index] );
    }
}

void EnemyShip::syncObjects( const sf::Window& referTo ) {
    for ( unsigned int index = 0 ; index < m_enemyShips.size() ; index++ ) {
        m_enemyShips[index]->syncObject( referTo );
    }
}

void EnemyShip::add( const sf::Vector2f& position , float fullHealth ) {
    m_enemyShips.push_back( new EnemyShip( position , fullHealth ) );
}

void EnemyShip::addBullet( unsigned int index , const sf::Window& referTo ) {
    Bullet::add( *m_enemyShips[index] , referTo , sf::Color( rand() % 256 , rand() % 256 , rand() % 256 ) );
}

void EnemyShip::controlShips( void* userData ) {
    for ( unsigned int index = 0 ; index < m_enemyShips.size() ; index++ ) {
        m_enemyShips[index]->controlShip( userData );
    }
}

void EnemyShip::controlShip( void* userData ) {
    ShipBase* playerShip = static_cast<ShipBase*>( userData );

    float angle = 90.f + atan2( body->GetPosition().y - playerShip->body->GetPosition().y , body->GetPosition().x - playerShip->body->GetPosition().x );
    float distance = (body->GetPosition() - playerShip->body->GetPosition()).Length();

    body->SetAngularVelocity( 0.f );

    // Adjust angle towards player
    body->SetTransform( body->GetPosition() , angle );

    // Adjust velocity towards player
    body->SetLinearVelocity( b2Vec2( 0.f , 10.f ) + b2Vec2( 3.5f * cos( angle ) , 3.5f * sin( angle ) ) );
}
