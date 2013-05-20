//=============================================================================
//File Name: Bullet.cpp
//Description: Handles all bullets created by entities
//Author: Tyler Veness
//=============================================================================

#include "Bullet.hpp"
#include "Ship.hpp"

std::vector<Bullet*> Bullet::bullets;

Bullet::~Bullet() {
    std::vector<Bullet*>::iterator index;
    for ( index = bullets.begin() ; *index != this ; index++ ) {
        if ( index >= bullets.end() ) {
            return;
        }
    }

    bullets.erase( index );
}

void Bullet::cleanup() {
    for ( unsigned int index = bullets.size() ; index > 0 ; index-- ) {
        delete bullets[0];
    }
}

void Bullet::drawAll( sf::RenderTarget& target , sf::RenderStates states ) {
    for ( unsigned int index = 0 ; index < bullets.size() ; index++ ) {
        target.draw( *bullets[index] );
    }
}

void Bullet::syncObjects( const sf::Window& referTo ) {
    for ( unsigned int index = 0 ; index < bullets.size() ; index++ ) {
        bullets[index]->syncObject( referTo );
    }
}

void Bullet::add( const Ship& ship , const sf::Window& referTo ) {
    bullets.push_back( new Bullet( ship , referTo ) );
}

void Bullet::checkCollisions( const Ship& ship , const sf::Window& referTo ) {
    b2Body* bulletBody;

    unsigned int index = 0;
    while ( index < bullets.size() ) {
        bulletBody = bullets[index]->body;

        if ( /*bulletBody->GetContactList() != NULL ||*/ sqrt( pow( ship.body->GetPosition().x - bulletBody->GetPosition().x , 2 ) + pow( ship.body->GetPosition().y - bulletBody->GetPosition().y , 2 ) ) > ( sqrt( pow( referTo.getSize().x / 2.f , 2.f ) + pow( referTo.getSize().y / 2.f , 2.f ) ) ) / 30.f ) { // if bullet hit something or it left the window
            delete bullets[index]; // bullet is outside of window view so delete it
            continue; // stop checking body collisions for this bullet because it doesn't exist anymore
        }
        else {
            index++;
        }
    }
}

Bullet::Bullet( const Ship& ship , const sf::Window& referTo ) :
        Box2DBase( &shape , BoxToSFML( ship.body->GetPosition().x + 1.5f * cos( ship.body->GetAngle() + b2_pi / 2.f ) , ship.body->GetPosition().y + 1.5f * sin( ship.body->GetAngle() + b2_pi / 2.f ) , referTo.getSize().y ) , b2_dynamicBody ) , shape( sf::Vector2f( 2.f , 10.f ) ) {
    float angle = ship.body->GetAngle();

    // Define the ground box shape.
    b2PolygonShape bulletBox;

    // The extents are the half-widths of the box.
    bulletBox.SetAsBox( 1.f / 30.f , 5.f / 30.f );

    // Add the bullet fixture to the bullet body.
    body->CreateFixture( &bulletBox , 1.f );
    //body->SetBullet( true );

    body->SetLinearVelocity( b2Vec2( 12.f * cos( angle + b2_pi / 2.f ) , 12.f * sin( angle + b2_pi / 2.f ) ) + ship.body->GetLinearVelocity() ); // sets bullet to travel at 12 plus how fast ship was going while shooting (conservation of momentum)
    body->SetTransform( body->GetPosition() , angle );

    shape.setFillColor( sf::Color( 255 , 255 , 0 ) );
    shape.setOrigin( 5.f , 1.f );
}
